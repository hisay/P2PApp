#include "CASocket.h"

CASocket::E_Skt	CASocket::SelectSkt()
{
	if (INVALID_SOCKET == m_skt) return CASocket::ES_EXCEPTION;

	fd_set r, w, e;
	FD_ZERO(&r); FD_ZERO(&e); FD_ZERO(&w);
	FD_SET(m_skt, &r);
	FD_SET(m_skt, &w);
	FD_SET(m_skt, &e);
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1;
	int n = select((int)m_skt + 1, &r, &w, &e, &tv);
	if (n == -1)
		return CASocket::ES_EXCEPTION;

	if (n == 0)
		return CASocket::ES_TIMEOUT;

	int rt = 0;
	if (FD_ISSET(m_skt, &r))
		rt |= CASocket::ES_RECV;
	if (FD_ISSET(m_skt, &w))
		rt |= CASocket::ES_SEND;
	if (FD_ISSET(m_skt, &e))
		rt = CASocket::ES_EXCEPTION;

	return (CASocket::E_Skt)rt;
}
bool CASocket::DoRecvData()
{
	if (!OnBeforRecv())return true;
	if (INVALID_SOCKET != m_skt)
	{
		while (true) {
			auto l = m_bufTmp.GetBuffLen();
			auto p = m_bufTmp.GetBuff();

			int n = recv(m_skt, (char*)p, (int)l, 0);
			if (-1 == n) {
				int err = WSAGetLastError();
				if (err == WSAEWOULDBLOCK)
					return true;

				cout << "CASocket::DoRecvData() failed err=" << err << endl;
				Close();
				return false;
			}

			if (n) {
				m_bufRecv.AddBuffer(p, (unsigned int)n);

				do {
					l = m_bufRecv.GetCufBuffLen();
					p = m_bufRecv.GetCurBuff();
					if (l < sizeof(PNS_Header))
						break;

					PNS_Header* h = (PNS_Header*)p;
					if (!CHECKFLAG(h))
						return false;

					if (h->len <= l)
					{
						this->OnRecvData(p, h->len);
						m_bufRecv.UseBuffer(h->len);
					}
					else {
						break;
					}

				} while (true);

				continue;
			}
			cout << " CASocket::DoRecvData() recv return = 0 ,now close socket! " << endl;
			Close();
			return false;
		}
	}
	return false;
}
bool CASocket::DoSendData()
{
	if (!OnBeforSend()) { return true; }
	if (INVALID_SOCKET != m_skt)
	{
		while (true) {
			auto l = m_bufSend.GetCufBuffLen();
			auto p = m_bufSend.GetCurBuff();
			if (!l) { return true; }

			int n = send(m_skt, (char*)p, (int)l, 0);
			if (-1 == n) {
				if (WSAGetLastError() == WSAEWOULDBLOCK)
					return true;
				cout << " CASocket::DoSendData() failed err = " << WSAGetLastError() << endl;
				Close();
				return false;
			}

			if (n) {
				m_bufSend.UseBuffer((unsigned int)n);
				OnSendDataOver((UINT)n);
			}
			else { 
				cout << " CASocket::DoSendData() send return = 0 ,now close socket! " << endl;
				Close(); return false; }
		}
	}
	return false;
}

bool CASocket::SendData(unsigned char* pBuf, unsigned int len)
{
	if (INVALID_SOCKET != m_skt)
	{
		return m_bufSend.AddBuffer(pBuf, len);
	}
	return false;
}

bool CASocket::Update()
{
	auto s = SelectSkt();
	switch (s)
	{
	case ES_RECV:
		if (!DoRecvData()) return false;
		break;
	case ES_SEND:
		if (!DoSendData()) return false;
		break;
	case ES_RECV_SEND:
		if (!DoRecvData() || !DoSendData()) return false;
		break;
	case ES_EXCEPTION:
		return false;
	}
	return true;
}

bool CASocket::GetPeerAddr(unsigned long& ul, unsigned short& port)
{
	return GetPeerAddr(m_skt, ul, port);
}
bool CASocket::GetSelfAddr(unsigned long& ul, unsigned short& port)
{
	return GetSelfAddr(m_skt, ul, port);
}

bool CASocket::GetPeerAddr(SOCKET& skt, unsigned long& ul, unsigned short& port)
{
	SOCKADDR_IN addr;
	int s=sizeof(addr);
	if (!getpeername(skt, (sockaddr*)&addr, &s))
	{
		ul = addr.sin_addr.S_un.S_addr;
		port = addr.sin_port;
		return true;
	}
	return false;
}
bool CASocket::GetSelfAddr(SOCKET& skt, unsigned long& ul, unsigned short& port)
{
	SOCKADDR_IN addr;
	int s = sizeof(addr);
	if (!getsockname(skt, (sockaddr*)&addr, &s))
	{
		ul = addr.sin_addr.S_un.S_addr;
		port = addr.sin_port;
		return true;
	}
	return false;
}