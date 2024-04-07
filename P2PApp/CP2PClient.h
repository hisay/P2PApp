#pragma once
#include "CASocket.h"
#include <string>


class CP2PClientReq;
class CP2PClientRes;
class CP2PClient;
struct IP2PAdapter
{
    virtual void GetSvrIP(string& ip, UINT& port) = 0;
    virtual bool SendMainLinkData(PBYTE pBuf, UINT nSize) = 0;
    virtual CP2PClient* GetThisPtr() = 0;
};

class CP2PClient :
    public CASocket,public IP2PAdapter
{
public:
    enum E_MSG {
        EM_QUERY_ID = 1,
        EM_CONN_ID,
        EM_CONNSVR,
        EM_EXITAPP,
        EM_SENDMSG
    };
    enum E_CltType {
        ECT_MainLink,
        ECT_P2PReq,
        ECT_P2PRes
    };
    CP2PClient() { ::InitializeCriticalSection(&m_cs); m_eSktType = EST_P2PCLIENT; }
    virtual ~CP2PClient();
    virtual bool Connect(const char* ip, UINT port);
    virtual bool Update();
    virtual bool OnBeforConn() { return true; }
    virtual bool OnBeforSend();
    virtual void OnConnect();
    virtual void OnP2PMsg(E_MSG_P2P msg, ULONG p1, ULONG p2);
    virtual void SendMsg(E_MSG_P2P msg, ULONG p1, ULONG p2);
    virtual void Close() { __super::Close(); m_isConnect = false;  }
    virtual void OnRecvData(unsigned char* pBuf, unsigned int len);
    virtual void GetSvrIP(string& ip, UINT& port) { ip = m_strLastSvrIP; port = m_nLastSvrPort; }
    virtual bool SendMainLinkData(PBYTE pBuf, UINT nSize) { return this->SendData(pBuf, nSize); }
    virtual void OnP2PRes(SOCKADDR_IN& addr,DWORD tid,DWORD selfid);
    virtual SOCKADDR_IN& GetP2PAddr() { return m_p2pAddr; }
    void SendMessage(string& m);
    virtual void  OnMsgProc(string& m);
    virtual void  OnP2PListenConnOK(SOCKET skt);
    virtual CP2PClient* GetThisPtr() { return this; }
protected:
    void ClearP2PSkt();
    vector<string> m_vMsg;
    vector<CASocket*> m_vP2PSkt;
    bool m_isConnect{ false };

    DWORD   m_idtId{ 0 };

    SOCKADDR_IN m_p2pAddr{ .sin_family=AF_INET };
    CRITICAL_SECTION m_cs;
    E_CltType m_cltType{ ECT_MainLink };

    string m_strLastSvrIP;
    UINT   m_nLastSvrPort{ 0 };
};

