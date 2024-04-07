#include "CSktMng.h"

bool CSktMng::Update()
{
	if (m_vSkt.empty()) return false;

	for (auto i = m_vSkt.begin(); i != m_vSkt.end();)
	{
		if ((*i)->Update())
		{
			i++;
		}
		else {
			delete* i;
			i = m_vSkt.erase(i);
		}
	}
	return true;
}