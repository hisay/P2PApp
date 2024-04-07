#pragma once
#include "CASocket.h"
class CListener :
    public CASocket
{
public:
    CListener( );

    virtual bool Listener(int port);
    virtual bool OnBeforRecv();
    virtual void OnBeforBind(SOCKADDR_IN& addr) {}
    virtual void OnAccept(SOCKET skt) { ; }

protected:
    SOCKET m_skt{ INVALID_SOCKET };
    int     m_port{ 0 };
};

