#include "CP2PClientRes.h"
void CP2PClientRes::OnConnect()
{
	__super::OnConnect();
	switch (m_eps)
	{
	case CP2PClientRes::EP_ConningSvr:
		m_eps = EP_ConnSvrOK;
		break;
	case CP2PClientRes::EP_P2PConning:
		m_eps = EP_P2POK;
		break;
	}

	if (m_eps == EP_ConnSvrOK)
	{
		auto cid = MAKE_PNS_ConnId(m_idtId, m_targetId);
		this->SendData((PBYTE)&cid,sizeof(cid));
	}
}
bool CP2PClientRes::OnBeforSend() {
	if (m_eps == EP_ConnSvrOK)
	{
		m_eps = EP_SendingReq;
	}
	return __super::OnBeforSend();
//	return true;
}

void  CP2PClientRes::OnP2PListenConnOK(SOCKET skt)
{
	if (m_pParent)
		m_pParent->GetThisPtr()->OnP2PListenConnOK(skt);
	else
		closesocket(skt);
	this->m_eps = EP_P2POK;
}

bool CP2PClientRes::Update() {
	bool bl = m_listener.Update();
	bool bok = __super::Update();

	if (!bok) {
		Close();
		if (bl) return true;
		if (m_bFirstP2PConn) {
			auto cf = MAKE_PNSP2PResConnFailed(m_idtId, m_targetId);
			this->m_pParent->SendMainLinkData((PBYTE)&cf,sizeof(cf));
			m_listener.Listener(htons(m_p2pAddr.sin_port));
			m_bFirstP2PConn = false;
			/*if (!m_listener.IsOK())
				m_listener.Listener(htons(m_p2pAddr.sin_port));
			else
			{
				if (!(m_eps != EP_SendOK && m_eps != EP_P2PConning)) {
					OnP2PConnect(m_remoteAddr.sin_addr.S_un.S_addr, m_remoteAddr.sin_port);
				}
			}*/
		}
	}
	else {
		if (!m_isConnect && m_bFirstP2PConn)
		{
			if (GetTickCount64() - m_cTick > 5000 && !m_listener.IsOK())
				Close();
		}
	}

	return bok || bl;
}
void CP2PClientRes::OnSendDataOver(unsigned int ul)
{
	if (m_eps == EP_SendingReq)
	{
		auto n = m_bufSend.GetCufBuffLen();
		if (!n) {
			//Close();
			m_eps = EP_SendOK;
			////m_listener.Listener(htons(m_p2pAddr.sin_port));
		}
	}
}


bool CP2PClientRes::OnP2PConnect(ULONG ul, USHORT us)
{
	if (!m_bFirstP2PConn)
		m_bFirstP2PConn = true;
	//if (m_eps != EP_SendOK && m_eps != EP_P2PConning) {
	//	cout << "CP2PClientRes::OnP2PConnect dn't ready recv remote ip!" << endl; return false;
	//}
	m_remoteAddr.sin_addr.S_un.S_addr = ul;
	m_remoteAddr.sin_port = us;
	auto addr = this->GetIPAddress(ul);
	//m_listener.Close();
	return Connect(addr.c_str(),htons( us));
}

bool CP2PClientRes::CheckMe(DWORD selfId, DWORD targetid)
{
	return m_idtId == selfId && m_targetId == targetid && (m_eps == EP_SendingReq || m_eps == EP_SendOK);
}

bool CP2PClientRes::Connect(const char* ip, UINT port)
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
	}
	return __super::Connect(ip, port);
}

bool CP2PClientRes::OnBeforConn()
{
	if (EP_P2PConning == m_eps)
	{
		bool ok = 0 == bind(m_skt, (sockaddr*)&m_p2pAddr, sizeof(m_p2pAddr));
		if (!ok) {
			cout << "  CP2PClientRes::OnBeforConn() bind " << htons(m_p2pAddr.sin_port) << " failed ! err=" << WSAGetLastError() << endl;
		}
		else {
			cout << "  CP2PClientRes::OnBeforConn() bind port on:" << htons(m_p2pAddr.sin_port) << endl;
		}
		return ok;
	}
	return true;
}

bool CP2PClientRes::CanDel()
{
	if (m_eps == EP_P2PConning || m_eps == EP_SendOK )
		return false;
	return true;
}

void CP2PClientRes::OnP2PMsg(E_MSG_P2P msg, ULONG p1, ULONG p2)
{
	switch (msg)
	{
	case E_MSG_P2P::EMP_CONN_JUST_OK:
		//m_listener
		Close();
		break;
	case E_MSG_P2P::EMP_REQ_LISTEN_OK:
		OnP2PConnect(this->m_remoteAddr.sin_addr.S_un.S_addr, (this->m_remoteAddr.sin_port));
		break;
	}
}