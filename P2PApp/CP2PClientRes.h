#pragma once
#include "CP2PClient.h"
#include "CP2PClientListener.h"
class CP2PClientRes :
    public CP2PClient
{
public:
    enum E_PCStatus
    {
        EP_Unconn,
        EP_ConningSvr,
        EP_ConnSvrOK,
        EP_SendingReq,
        EP_SendOK,
        EP_P2PConning,
        EP_P2POK,
        EP_P2PFailed,
        EP_Closed
    };
    virtual bool OnP2PConnect(ULONG uladdr, USHORT port);
    virtual void OnConnect();
    virtual bool OnBeforSend();
    virtual void OnSendDataOver(unsigned int);
    virtual bool OnBeforConn();
    virtual bool Update();
    virtual bool CanDel();
    virtual bool Connect(const char* ip, UINT port);
    virtual void OnP2PMsg(E_MSG_P2P msg, ULONG p1, ULONG p2);
    virtual void  OnP2PListenConnOK(SOCKET skt);
    bool CheckMe(DWORD selfId, DWORD targetid);
    CP2PClientRes(IP2PAdapter* pPtr, DWORD selfId,DWORD targetId) { m_cltType = ECT_P2PRes; m_pParent = pPtr; m_eSktType = EST_P2PCLIENTRES;
    m_idtId = selfId;
    this->m_targetId = targetId;
    }
    virtual void SetP2PRemoteAddr(ULONG ul, USHORT p) {
        m_remoteAddr.sin_addr.S_un.S_addr = ul;
        m_remoteAddr.sin_port = p;
    }
 
protected:
    CP2PClientListener m_listener{ this };
    DWORD   m_targetId{ 0 };
    E_PCStatus  m_eps{ EP_Unconn };
    IP2PAdapter* m_pParent{ nullptr };
    SOCKADDR_IN m_remoteAddr{ .sin_family = AF_INET };
    DWORD64 m_cTick{ 0 };
    bool m_bFirstP2PConn{ false };
};

