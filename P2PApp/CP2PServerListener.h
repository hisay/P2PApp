#pragma once
#include "CListener.h"
#include "CP2PServerSocket.h"
#include "p2pclt.h"

class CP2PServerListener :
    public CListener, public ISktMng
{
public:
    enum E_PSL_Type
    {
        EPT_MAIN_LINK,
        EPT_P2P_LINK
    };
    CP2PServerListener(){ m_eSktType = EST_P2PSERVERLISTENER; }
    virtual void OnAccept(SOCKET m);
    virtual bool Update();
    virtual CASocket* FindSocketById(DWORD id);
    virtual void GetAllSocketId(DWORD*& p, int& l) ;
    virtual ISktMng* GetMainLink();
    void SetP2PLink(CP2PServerListener* pMain) { m_epType = EPT_P2P_LINK; m_pMainLink = pMain; }
protected:
    vector<CP2PServerSocket*> m_vClt;
    CP2PServerListener* m_pMainLink{ nullptr };
    E_PSL_Type  m_epType{ EPT_MAIN_LINK };
};

