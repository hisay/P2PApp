#pragma once
#include "CASocket.h"
#include "p2pclt.h"

class CP2PServerSocket :
    public CASocket
{
public:
    CP2PServerSocket(SOCKET k) :CASocket(k) { m_eSktType = EST_P2PSERVERSOCKET; }
    virtual void OnRecvData(unsigned char* pBuf, unsigned int len);
    void SetSktMng(ISktMng* p) { m_pMng = p; }
    DWORD GetId() { return m_idtID; }
protected:
    void SendLoginRes();
    void OnConnId(PNS_ConnId* pc);
    DWORD m_idtID{ 0 };
    ISktMng* m_pMng{ nullptr };
};

