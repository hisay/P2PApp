#pragma once
#include "inc.h"
#include "CBuffer.h"
#include <WS2tcpip.h>
class CASocket
{
public:
	enum E_SockType
	{
		EST_ASOCKET,
		EST_LISTENER,
		EST_P2PCLIENTLISTENER,
		EST_P2PSERVERLISTENER,
		EST_P2PSERVERSOCKET,
		EST_P2PCLIENT,
		EST_P2PCLIENTREQ,
		EST_P2PCLIENTRES,
		EST_P2PCLIENTSKT
	};
	CASocket() {}
	CASocket(SOCKET h)  {
		SetSocket(h);
	}
	virtual ~CASocket() { Close(); }
	virtual bool Update();
	virtual void Close() { if (INVALID_SOCKET != m_skt) { closesocket(m_skt); m_skt = INVALID_SOCKET; } }
	virtual bool CanDel() { return true; }
	bool SendData(unsigned char* pBuf, unsigned int len);
	virtual void OnRecvData(unsigned char* pBuf, unsigned int len) { ; }
	virtual bool OnBeforSend() { return true; }
	virtual bool OnBeforRecv() { return true; }
	virtual void OnSendDataOver(unsigned int) { ; }
	virtual void SendMessage(string& msg){}
	bool GetPeerAddr(unsigned long& ul, unsigned short& port);
	bool GetSelfAddr(unsigned long& ul, unsigned short& port);
	bool GetPeerAddr(SOCKET& skt,unsigned long& ul, unsigned short& port);
	bool GetSelfAddr(SOCKET& skt,unsigned long& ul, unsigned short& port);
	void SetSocket(SOCKET h)
	{
		m_skt = h;
		u_long	l = 1;
		::ioctlsocket(h, FIONBIO, &l);

		linger so_linger;so_linger.l_onoff = 1;so_linger.l_linger = 0;
		setsockopt(m_skt, SOL_SOCKET, SO_LINGER,(const char*) & so_linger, sizeof (so_linger));
	}
	E_SockType GetST() { return m_eSktType; }
	string GetIPAddress(ULONG laddr) {
 
		SOCKADDR_IN addr = { 0 };
		addr.sin_addr.S_un.S_addr = laddr;
		addr.sin_family = AF_INET;

		char buffer[INET_ADDRSTRLEN];

		// 假设 addr 已经被正确初始化
		const char* address = inet_ntop(AF_INET, &addr.sin_addr, buffer, INET_ADDRSTRLEN);
		return string(address);
	}
	enum E_Skt {
		ES_RECV			=	0x1,
		ES_SEND			=	0x2,
		ES_RECV_SEND	=	0x3,
		ES_TIMEOUT		=	0x4,
		ES_EXCEPTION	=	0x8
		
	};
protected:

	E_Skt	SelectSkt();
	bool DoRecvData();
	bool DoSendData();
	SOCKET	m_skt{ INVALID_SOCKET };
	CBuffer	m_bufSend, m_bufRecv, m_bufTmp{ 1024 };
	E_SockType	m_eSktType{ EST_ASOCKET };
};

