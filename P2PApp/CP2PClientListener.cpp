#include "CP2PClientListener.h"

void CP2PClientListener::OnAccept(SOCKET skt)
{
	
	ULONG ul; USHORT port;
	this->GetPeerAddr(skt,ul, port);
	auto addr = this->GetIPAddress(ul);
	cout << "CP2PClientListener::OnAccept " << addr << ":" << htons(port) << endl;
	if (m_pParent)
		m_pParent->OnP2PListenConnOK(skt);
	else
		closesocket(skt);
	Close();
}

bool CP2PClientListener::Update()
{
	if (!__super::Update())
	{
		Close();
		return false;
	}
	else {
		return true;
	}
}