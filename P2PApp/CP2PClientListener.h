#pragma once
#include "CListener.h"
#include "CP2PClient.h"

class CP2PClientListener :
    public CListener
{
public:
    CP2PClientListener(CP2PClient* pClient)
    {
        m_pParent = pClient;
        m_eSktType = EST_P2PCLIENTLISTENER;
    }
    virtual void OnBeforBind(SOCKADDR_IN& addr) {
        auto b = m_pParent->GetP2PAddr();
        addr.sin_addr.S_un.S_addr = b.sin_addr.S_un.S_addr;
        addr.sin_port = b.sin_port;
        addr.sin_family = b.sin_family;
    }
    virtual void Close() { __super::Close(); m_isOk = false; }

    virtual bool Listener(int port) {
        if (__super::Listener(port)) { 
            m_isOk = true;
            return true; 
        }
        else {
            m_isOk = false; return false;
        }
    }
    virtual bool Update();
    virtual void OnAccept(SOCKET skt);
    bool IsOK() { return m_isOk; }
protected:
    CP2PClient* m_pParent{ nullptr };
    bool m_isOk{ false };
};

