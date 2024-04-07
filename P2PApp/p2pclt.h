#pragma once
#include "CASocket.h"

struct ISktMng
{
	virtual CASocket* FindSocketById(DWORD id) = 0;
	virtual void GetAllSocketId(DWORD*& p, int& l) = 0;
	virtual ISktMng* GetMainLink() = 0;
};