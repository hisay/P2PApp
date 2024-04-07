#include "CP2PServerSocket.h"

DWORD g_UID = 1;
DWORD MakeID()
{
	auto x = g_UID++;
	if (!x)
		x = g_UID++;
	return x;
}
void CP2PServerSocket::OnRecvData(unsigned char* pBuf, unsigned int len)
{
	PNS_Header* h = (PNS_Header*)pBuf;
	cout << "ON recvType:" << h->type << endl;
	switch (h->type)
	{

	case PT_LOGIN:
		SendLoginRes();
		break;
	case PT_HEART:

		break;
	case PT_CONN_ID:
	{
		PNS_ConnId* c = (PNS_ConnId*)h;
		OnConnId(c);
		//Close();
	}
		break;
	case PT_P2PMSG:
	{
		auto p = (PNS_P2PMsg*)h;
		auto t = m_pMng->FindSocketById(p->P2);
		if (t) {
			t->SendData(pBuf, len);
		}
	}
		break;
	case PT_CONN_ID_RES:
	{
		if (!m_pMng)break;
		PNS_ConnIdRes* c = (PNS_ConnIdRes*)h;
		auto pS = m_pMng->FindSocketById(c->targetId);
		if (!pS)break;
		auto res = MAKE_PNS_ConnIdRes(0, 0,c->selfId,c->targetId);
		if (this->GetPeerAddr(res.addr, res.port))
		{
			pS->SendData((PBYTE)&res, sizeof(res));
		}
	}
		break;
	case PT_P2PRESCONNFAIL:
	{
		auto r = (PNS_P2PResConnFailed*)h;
		auto pt = m_pMng->FindSocketById(r->targetId);
		if (!pt)break;
		pt->SendData(pBuf,len);
	}
		break;
	case PT_QUERY_ID:
	{
		if (!m_pMng)break;
		DWORD* p = 0;
		int l = 0;
		m_pMng->GetAllSocketId(p, l);
		UINT tLen = l * sizeof(DWORD) + sizeof(PNS_QueryIdRes);
		PBYTE b = new BYTE[tLen];
		PNS_QueryIdRes* pqi = (PNS_QueryIdRes*)b;

		pqi->header.flag = PN_HEADER;
		pqi->header.len = tLen;
		pqi->header.type = PT_QUERY_ID_RES;
		pqi->size = l;
		memcpy(pqi->val, p, sizeof(DWORD) * l);

		this->SendData(b, tLen);
		delete[]p;
		delete[]b;
	}
		break;
	case PT_CONN_ID_REQRES:
	{
		if (!m_pMng || !m_pMng->GetMainLink())break;
		auto p = (PNS_ConnIdReqRes*)h;
		
		auto pS = m_pMng->GetMainLink()->FindSocketById(p->tid);
		if (!pS)break;
		auto res = MAKE_PNS_ConnIdRes(0, 0,p->selfId,p->tid);
		//res.selfId = p->selfId;
		if (this->GetPeerAddr(res.addr, res.port))
		{
			pS->SendData((PBYTE)&res, sizeof(res));
		}

		auto msg = MAKE_PNSMsg(E_MSG_P2P::EMP_CONN_REQ_JUST_OK);
		this->SendData((PBYTE)&msg, sizeof(msg));

		//Close();
		//auto pc = pS->GetP2PRes()
	}
		break;
	}
}

void CP2PServerSocket::OnConnId(PNS_ConnId* pc)
{
	if (!m_pMng) return;

	if (!m_pMng->GetMainLink()) return;

	auto tsk = m_pMng->GetMainLink()->FindSocketById(pc->targetId);
	auto r = MAKE_PNS_ConnIdRes(0, 0,pc->targetId,pc->selfId);
	if (!tsk) {
		r.ok = false;
		auto pthis = m_pMng->GetMainLink()->FindSocketById(pc->selfId);
		pthis&& pthis->SendData((unsigned char*)&r, sizeof(r));
		Close();
 		return;
	}
	else {

		if (tsk->GetPeerAddr(r.addr, r.port))
		{
			r.ok = true;
			
			auto cq = MAKE_PNS_ConnIdReq(0, 0,pc->selfId);
			if (this->GetPeerAddr(cq.addr, cq.port))
				tsk->SendData((unsigned char*)&cq, sizeof(cq));
			
		}
		else
			r.ok = false;
	}
	auto msg = MAKE_PNSMsg(E_MSG_P2P::EMP_CONN_JUST_OK);
	this->SendData((PBYTE)&msg, sizeof(msg));
}

void CP2PServerSocket::SendLoginRes()
{
	SOCKADDR_IN addr = { 0 };
	int alen = sizeof(addr);

	getpeername(m_skt, (sockaddr*)&addr, &alen);
	m_idtID = MakeID();
	auto res = MAKE_PNSLoginRes(m_idtID, addr.sin_addr.S_un.S_addr, addr.sin_port);

	if (!this->SendData((unsigned char*)&res, sizeof(res)))
		Close();
}