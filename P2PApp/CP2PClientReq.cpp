#include "CP2PClientReq.h"


void CP2PClientReq::OnConnect()
{
	__super::OnConnect();
	switch (m_eps)
	{
	case CP2PClientReq::EP_ConningSvr:
		m_eps = EP_ConnSvrOK;

		break;
	case CP2PClientReq::EP_P2PConning:
	case CP2PClientReq::EP_P2P_ReqConning:
		m_eps = EP_P2POK;
		break;
	}
	if (m_eps == EP_ConnSvrOK)
	{
		auto cid = MAKE_PNS_ConnIdReqRes(m_idtId, m_targetId);
		this->SendData((PBYTE)&cid, sizeof(cid));
	}
}
bool CP2PClientReq::OnBeforConn()
{
	if (EP_P2PConning == m_eps || EP_P2P_ReqConning == m_eps)
	{
		bool ok = 0 == bind(m_skt, (sockaddr*)&m_p2pAddr, sizeof(m_p2pAddr));
		if (!ok) {
			cout << " CP2PClientReq::OnBeforConn() bind " << htons(m_p2pAddr.sin_port) << " failed ! err=" << WSAGetLastError() << endl;
		}
		else {
			cout << " CP2PClientReq::OnBeforConn() bind port on:" << htons(m_p2pAddr.sin_port) << endl;
		}
		return ok;
	}
	return true;
}
bool CP2PClientReq::OnBeforSend() {
	if (m_eps == EP_ConnSvrOK)
	{
		m_eps = EP_SendingReq;
	}
	return __super::OnBeforSend();
}

bool CP2PClientReq::CanDel()
{
	if (m_eps == EP_P2PConning || m_eps == EP_SendOK || m_eps==EP_P2P_ReqConning || m_eps== EP_P2P_ListenWait)
		return false;
	return true;
}

bool CP2PClientReq::Connect(const char* ip, UINT port)
{
	m_cTick = GetTickCount64();
	switch (m_eps)
	{
	case EP_Unconn:
		m_eps = EP_ConningSvr;
		break;
	case EP_ConnSvrOK:
	case EP_SendOK:
		m_eps = EP_P2PConning;
		break;
	case EP_P2P_ReqConning:
		break;
	}
	return __super::Connect(ip, port);
}

void CP2PClientReq::OnSendDataOver(unsigned int ul)
{
	if (m_eps == EP_SendingReq)
	{
		auto n = m_bufSend.GetCufBuffLen();
		if (!n) {
			//Close();
			m_eps = EP_SendOK;
			//m_listener.Listener(htons(m_p2pAddr.sin_port));
		}
	}
}
bool CP2PClientReq::Update() {
	bool bL = m_listener.Update();
	bool bok = __super::Update();
	if (!bok)
	{
		if (EP_P2P_ListenWait == m_eps)
		{
			return true;
		}
		if (m_eps == EP_P2P_ReqConning || m_eps == EP_P2PConning)
		{
			cout << "P2P Req conning failed" << endl;
			m_eps = EP_Closed;
			Close();
			//this->OnP2PConnect(m_remoteAddr.sin_addr.S_un.S_addr, m_remoteAddr.sin_port);
		}
		else {
			Close();
		}
		//if (m_bFirstP2PConn) {
		//	if (!m_listener.IsOK())
		//		m_listener.Listener(htons(m_p2pAddr.sin_port));
		//	else {
		//		if (m_eps == EP_SendOK || EP_P2PConning == m_eps)
		//		{
		//			OnP2PConnect(m_remoteAddr.sin_addr.S_un.S_addr, m_remoteAddr.sin_port);
		//		}
		//	}
		//}
	}
	else {
		if (!m_isConnect &&(m_eps == EP_P2P_ReqConning || m_eps == EP_P2PConning))
		{
			if (GetTickCount64() - m_cTick > 5000 && !m_listener.IsOK())
				Close();
		}
	}
	return bok || bL;
}
void  CP2PClientReq::OnP2PListenConnOK(SOCKET skt)
{
	this->m_eps = EP_P2POK;
	if (m_pParent)
		m_pParent->GetThisPtr()->OnP2PListenConnOK(skt);
	else
		closesocket(skt);
}
bool CP2PClientReq::OnP2PConnect(ULONG ul, USHORT us)
{
	if (!m_bFirstP2PConn) { m_bFirstP2PConn = true; }
	m_remoteAddr.sin_addr.S_un.S_addr = ul;
	m_remoteAddr.sin_port = us;

	auto addr = this->GetIPAddress(ul);
	if (!Connect(addr.c_str(), htons(us))) {
				return false;
	}
	//if (m_eps == EP_ConnSvrOK || m_eps == EP_SendOK)
	//{
	//	Close();
	//	m_eps = EP_P2PConning;
	//	if (!Connect(addr.c_str(), htons(us)))
	//		return false;
	//}
	//else {
	//	m_eps = EP_ConningSvr;
	//	string ip;
	//	UINT port;
	//	m_pParent->GetSvrIP(ip, port);
	//	if (!Connect(ip.c_str(), port)) {
	//		return false;
	//	}
	//}
	return false;
}



void CP2PClientReq::OnP2PMsg(E_MSG_P2P msg, ULONG p1, ULONG p2)
{
	switch (msg)
	{
	case E_MSG_P2P::EMP_CONN_REQ_JUST_OK:
		Close();
		m_eps = EP_P2P_ListenWait;
		if (m_listener.Listener(htons(m_p2pAddr.sin_port)) && this->m_pParent)
			this->m_pParent->GetThisPtr()->SendMsg(EMP_REQ_LISTEN_OK, m_idtId, m_targetId);
		break;
	case E_MSG_P2P::EMP_REQ_P2P_CONN:
		if (p1 == this->m_targetId)
		{
			m_listener.Close();
			m_eps = EP_P2P_ReqConning;
			this->OnP2PConnect(m_remoteAddr.sin_addr.S_un.S_addr, m_remoteAddr.sin_port);
		}
		break;
	}
}