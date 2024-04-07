#include "CListener.h"
#include <iostream>
using namespace std;

CListener::CListener( )
{

	m_eSktType = EST_LISTENER;
}

bool CListener::Listener(int p)
{
	do {
		Close();
		m_skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		SetSocket(m_skt);

		SOCKADDR_IN addr = { 0 };
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(p);
		OnBeforBind(addr);

		if (bind(m_skt, (sockaddr*)&addr, sizeof(addr))) {
			cout << "Bind socket failed:" << WSAGetLastError() << endl;
			break;
		}
		else {
			cout <<m_skt<< "-bind port on:" << htons(addr.sin_port) << endl;
		}

		if (listen(m_skt, 5))
		{
			cout << "Listen socket failed:" << WSAGetLastError() << endl;
			break;
		}
		else {
			cout << "Listening port on :" << htons(addr.sin_port) << endl;
		}
		return true;
	} while (false);
	return false;
}
bool CListener::OnBeforRecv()
{
	auto x = accept(m_skt, 0, 0);
	if (INVALID_SOCKET == x)
	{
		Close();
	}
	else {
		OnAccept(x);
	}
	return false;
}
 