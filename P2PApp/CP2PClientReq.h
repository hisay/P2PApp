#pragma once
#include "CP2PClient.h"
#include "CP2PClientListener.h"
class CP2PClientReq :
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
        EP_Closed,
        EP_P2P_ReqConning,
        EP_P2P_ListenWait
    };

    virtual bool OnP2PConnect(ULONG uladdr, USHORT port);
    virtual void OnConnect();
    virtual bool OnBeforSend();
    virtual bool Update();
    virtual bool CanDel();
    virtual bool OnBeforConn();
    virtual void OnSendDataOver(unsigned int);
    virtual bool Connect(const char* ip, UINT port);
    virtual void OnP2PMsg(E_MSG_P2P msg, ULONG p1, ULONG p2);
    virtual void  OnP2PListenConnOK(SOCKET skt);
    CP2PClientReq(IP2PAdapter* pP,DWORD selfid,DWORD targetId) {
        m_cltType = ECT_P2PReq; m_pParent
            = pP;
        m_eSktType = EST_P2PCLIENTREQ;
        m_idtId = selfid;
        this->m_targetId = targetId;
    }
    void SetP2PRemoteAddr(ULONG ip, USHORT port) {
        m_remoteAddr.sin_addr.S_un.S_addr = ip;
        m_remoteAddr.sin_port = port;
    }
 
protected:
    CP2PClientListener m_listener{ this };
    DWORD   m_targetId{ 0 };
    E_PCStatus  m_eps{ EP_Unconn };
    IP2PAdapter* m_pParent{ nullptr };
    SOCKADDR_IN  m_remoteAddr{ .sin_family = AF_INET,.sin_port = 0,.sin_addr = 0,.sin_zero = {0} };
    DWORD64 m_cTick{ 0 };
    bool m_bFirstP2PConn{ false };
};

