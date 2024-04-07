#pragma once
#include "CASocket.h"
#include <vector>
using namespace std;

class CSktMng
{
public:
	~CSktMng() { CloseAll(); }
	void AddSocket(CASocket* p)
	{
		m_vSkt.push_back(p);
	}
	void CloseAll()
	{
		while (m_vSkt.size())
		{
			auto i = m_vSkt.begin();
			delete* i;
			m_vSkt.erase(i);
		}
	}
	bool Update();
protected:
	vector<CASocket*> m_vSkt;
};

