#include "CP2PClient.h"
#include "CP2PClientReq.h"
#include "CP2PClientRes.h"
#include "CP2PClientSkt.h"
CP2PClient::~CP2PClient() {
	{ ::DeleteCriticalSection(&m_cs); ClearP2PSkt(); }
}

bool CP2PClient::OnBeforSend()
{
	if (!m_isConnect)
	{
		m_isConnect = true;
		this->OnConnect();
		return false;
	}
	return true;
}
void CP2PClient::ClearP2PSkt()
{
	while (m_vP2PSkt.size())
	{
		auto x = m_vP2PSkt.begin();
		delete* x;
		m_vP2PSkt.erase(x);
	}
}

void  CP2PClient::OnP2PListenConnOK(SOCKET skt)
{
	auto p = new CP2PClientSkt(skt);
	m_vP2PSkt.push_back(p);
}
void CP2PClient::OnConnect()
{
	ULONG ul =  0 ,ul2=0;
	USHORT port=0,port2=0;
	this->GetPeerAddr(ul, port);
	this->GetSelfAddr(ul2, port2);
	auto addr = GetIPAddress(ul);
	auto addr2 = GetIPAddress(ul2);
	cout << "CP2PClient::OnConnect() ->" << addr<<":"<<htons(port)<< " local:"<<addr2<<":"<<htons(port2)<<endl;
	
	this->GetSelfAddr(m_p2pAddr.sin_addr.S_un.S_addr, m_p2pAddr.sin_port);
	if ( EST_P2PCLIENT == m_eSktType) {
		auto l = MAKE_PNSLogin(0, 0);
		this->GetSelfAddr(l.cltAddr, l.cltPort);
		this->SendData((PBYTE)&l, sizeof(l));
	}
}

void CP2PClient::OnRecvData(unsigned char* pBuf, unsigned int len) { 
	
	PNS_Header* h = (PNS_Header*)pBuf;
	cout << "ON recvType:" << h->type << endl;
	switch (h->type)
	{
	case PT_LOGIN_RES:
	{
		PNS_Login_Res* c = (PNS_Login_Res*)h;
		m_idtId = c->selfId;
		cout << "Me ID is:" << m_idtId << endl;
	}
		break;

	case PT_QUERY_ID_RES:
	{
		PNS_QueryIdRes* q = (PNS_QueryIdRes*)h;
		for (ULONG l = 0; l < q->size; l++) {
			cout << "->ID:" << q->val[l] << ( q->val[l] == m_idtId ? " [me]":"")<< endl;
		}
	}
		break;
	case PT_P2PMSG:
	{
		PNS_P2PMsg* m = (PNS_P2PMsg*)h;
		this->OnP2PMsg(static_cast<E_MSG_P2P>(m->msg), m->P1, m->P2);
	}
		break;
	case PT_CONN_ID_RES:
	{
		PNS_ConnIdRes* r = (PNS_ConnIdRes*)h;
		if (!r->ok)
		{
			cout << "CONN failed !" << " RemoteId:" << r->targetId << " meId:" << r->selfId << endl;
			break;
		}
		auto address = GetIPAddress(r->addr);
		cout <<"CONN_RES-> "<< (address) << ":" << htons(r->port) <<" RemoteId:"<< r->targetId<<" meId:"<<r->selfId << endl;

		SOCKADDR_IN addr = { 0 };
		addr.sin_addr.S_un.S_addr = r->addr;
		addr.sin_port = r->port;
		addr.sin_family = AF_INET;
		OnP2PRes(addr,r->targetId,r->selfId);
	}
		break;
	case PT_CONN_ID_REQ:
	{
		PNS_ConnIdReq* r = (PNS_ConnIdReq*)h;
		auto address = GetIPAddress(r->addr);
		USHORT port = htons(r->port);

		cout << "Recv p2p request on :" << address << ":" << port <<"->ID("<<r->sid<<")"<< endl;

		auto ps = new CP2PClientReq(this, this->m_idtId, r->sid);
		ps->SetP2PRemoteAddr(r->addr, r->port);
		if (ps->Connect(m_strLastSvrIP.c_str(), m_nLastSvrPort + 1))
			this->m_vP2PSkt.push_back(ps);
		else
			delete ps;
	}
		break;
	case PT_P2PRESCONNFAIL:
	{
		auto r = (PNS_P2PResConnFailed*)h;
		if (this->m_idtId == r->targetId)
		{
			for (auto x = m_vP2PSkt.begin(); x != m_vP2PSkt.end(); x++)
			{
				if ((*x)->GetST() == EST_P2PCLIENTREQ)
				{
					auto p = dynamic_cast<CP2PClientReq*>((*x));
					if (p) { p->OnP2PMsg(EMP_REQ_P2P_CONN,r->selfId,0); }
				}
			}
		}
		//auto pt = m_pMng->FindSocketById(r->targetId);

		//pt->SendData(pBuf, len);
	}
	break;
	case PT_DATA:
	{
		PNS_Data* pd = (PNS_Data*)h;
		cout << "RECV data: len=" << pd->len << endl;
		switch (pd->bType)
		{
		case 100:
			cout << "TextData:" << string((const char*)pd->buf, pd->len) << endl;
			break;
		}
	}
		break;
	}

	
}

void CP2PClient::OnP2PRes(SOCKADDR_IN& addr, DWORD tid, DWORD selfid)
{
	for (auto x = m_vP2PSkt.begin(); x != m_vP2PSkt.end(); x++) {
		if ((*x)->GetST() == EST_P2PCLIENTRES)
		{
			auto p = dynamic_cast<CP2PClientRes*>((*x));
			if (p) {
				if (p->CheckMe(selfid, tid)) {
					p->SetP2PRemoteAddr(addr.sin_addr.S_un.S_addr, addr.sin_port);
					
				}
			}
		}
	 }
}



void CP2PClient::SendMessage(string& m)
{
	::EnterCriticalSection(&m_cs);
	m_vMsg.push_back(m);
	::LeaveCriticalSection(&m_cs);
}

bool CP2PClient::Connect(const char* ip, UINT port)
{
	Close();
	
	m_skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET != m_skt)
	{
		SetSocket(m_skt);

		int reuse = 1;
		setsockopt(m_skt, SOL_SOCKET, SO_REUSEADDR,(const char*) & reuse, sizeof(reuse));

		if (!OnBeforConn())
			return false;
		SOCKADDR_IN addr = { 0 };
		inet_pton(AF_INET, ip, &addr.sin_addr.S_un.S_addr);
		 
		addr.sin_port = htons(port);
		addr.sin_family = AF_INET;
		this->m_strLastSvrIP = ip;
		this->m_nLastSvrPort = port;
		int n = connect(m_skt, (sockaddr*)&addr, sizeof(addr));
		if (n == -1)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				return true;
			return false;
		}
		return true;
	}
	return false;
}

void CP2PClient::OnMsgProc(string& msg)
{
	PBYTE p = (PBYTE)msg.c_str();
	UINT n = *(UINT*)p;
 
	p = PBYTE((PUINT)p + 1);
	size_t dn = msg.size() - sizeof(UINT);
	switch (n)
	{
	case EM_QUERY_ID:
	{
		auto q = MAKE_PNS_QueryId(this->m_idtId);
		this->SendData((PBYTE)&q, sizeof(q));
	}
		break;
	case EM_CONN_ID:
	{
		DWORD tid = *(DWORD*)p;
		if (tid == m_idtId)
		{
			cout << "cant connect self!" << endl;
			break;
		}
		//auto a = MAKE_PNS_ConnId(m_idtId, tid);
		//this->SendData((PBYTE)&a, sizeof(a));

		auto pskt = new CP2PClientRes(this, m_idtId, tid);
		if (pskt->Connect(m_strLastSvrIP.c_str(), m_nLastSvrPort + 1))
			m_vP2PSkt.push_back(pskt);
		else
			delete pskt;
	}
		break;
	case EM_SENDMSG:
	{
		n = *(UINT*)p;

		p = PBYTE((PUINT)p + 1);
		dn -= sizeof(UINT);
		UINT nLen = sizeof(PNS_Data) + n;
		PBYTE pb = new BYTE[nLen];

		PNS_Data* ph = (PNS_Data*)pb;
		MAKE_PNS_Header(&ph->header, nLen, PT_DATA);
		ph->bType = 100;
		ph->len = n;
		memcpy(ph->buf, p, n);
		for (auto i = 0; i < m_vP2PSkt.size(); i++) {
			m_vP2PSkt[i]->SendData(pb, nLen);
		}
		delete []pb;
	}
		break;
	case EM_CONNSVR:
	{
		string ipport(string((char*)p, dn));
		string ip = ipport.substr(0, ipport.find(':'));
		int port = atoi(ipport.substr(ipport.find(':') + 1).c_str());

		this->Connect(ip.c_str(),(UINT) port);
	}
		break;
	case EM_EXITAPP:
		Close();
		break;
	}
}
bool CP2PClient::Update()
{

	if (!__super::Update())
		return false;

	while (true) {
		bool b = true;
		::EnterCriticalSection(&m_cs);
		if (m_vMsg.size())
		{
			OnMsgProc(*m_vMsg.begin());
			m_vMsg.erase(m_vMsg.begin());
		}
		else {
			b = false;
		}
		::LeaveCriticalSection(&m_cs);
		if (!b)break;
	}

	auto cs = m_vP2PSkt.size();
	for (auto i = m_vP2PSkt.begin(); i !=  m_vP2PSkt.end(); ) {
		if (!(*i)->Update() && (*i)->CanDel()) {
			delete* i;
			i = m_vP2PSkt.erase(i);
			continue;
		}
		if (cs != m_vP2PSkt.size()) {
			i = m_vP2PSkt.begin();
			cs = m_vP2PSkt.size();
		}
		else {
			i++;
		}
		//if (!(*i)->Update() && (*i)->CanDel()) {
		//	i=m_vP2PSkt.erase(i);
		//}
		//else {
		//	i++;
		//}
	}
	return true;
}

void CP2PClient::SendMsg(E_MSG_P2P msg, ULONG p1, ULONG p2)
{
	auto m = MAKE_PNSMsg2(msg, p1, p2);
	this->SendData((PBYTE)&m, sizeof(m));
}

void CP2PClient::OnP2PMsg(E_MSG_P2P msg, ULONG p1, ULONG p2)
{
	for (auto x = m_vP2PSkt.begin(); x != m_vP2PSkt.end(); x++)
	{
		switch ((*x)->GetST())
		{
		case EST_P2PCLIENT:
		case EST_P2PCLIENTREQ:
		case EST_P2PCLIENTRES:
			auto p = dynamic_cast<CP2PClient*>((*x));
			if (p) p->OnP2PMsg(msg, p1, p2);
			break;
		}
	}
}