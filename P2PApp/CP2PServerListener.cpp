#include "CP2PServerListener.h"

void CP2PServerListener::OnAccept(SOCKET skt)
{
	ULONG ul;
	USHORT us;
	this->GetPeerAddr(skt, ul, us);
	auto addr = this->GetIPAddress(ul);
	cout << "Accept new client connect on " << addr << ":" << htons(us) << endl;
	auto p = new CP2PServerSocket(skt);
	p->SetSktMng(this);
	m_vClt.push_back(p);
}

bool CP2PServerListener::Update()
{
	if (!__super::Update())
		return false;
	for (auto i = m_vClt.begin(); i != m_vClt.end();)
	{
		if ((*i)->Update())
			i++;
		else {
			cout << (*i)->GetId() << " has exit!" << endl;
			delete* i;
			i = m_vClt.erase(i);
		}
	}
	return true;
}

CASocket* CP2PServerListener::FindSocketById(DWORD id)
{
	for (auto i = m_vClt.begin(); i != m_vClt.end(); i++)
	{
		if ((*i)->GetId() == id)
			return *i;
	}
	return nullptr;
}

#pragma warning(disable:6386)
void CP2PServerListener::GetAllSocketId(DWORD*& p, int& l)
{
	p = new DWORD[m_vClt.size()];
	l =(int) m_vClt.size();
	for (auto i = 0;i<m_vClt.size();i++)
	{
		p[i] = m_vClt[i]->GetId();
	}
}
#pragma warning(default:6386)

ISktMng* CP2PServerListener::GetMainLink()
{
	if (m_epType == EPT_P2P_LINK && m_pMainLink)
		return m_pMainLink;
	return nullptr;
}