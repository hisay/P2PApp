#pragma once
#include "CP2PClient.h"
class CP2PClientSkt :
    public CP2PClient
{
public:
    CP2PClientSkt(SOCKET skt) {
        SetSocket(skt);
        m_eSktType = EST_P2PCLIENTSKT;
    }
};

