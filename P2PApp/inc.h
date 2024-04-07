#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <iostream>
using namespace std;
#include "protocol.h"

#pragma comment(lib, "ws2_32.lib")

struct SocketInit {
	static bool Init() {
		WORD w = MAKEWORD(2, 2);
		WSAData d;
		return 0 == WSAStartup(w, &d);
	}
};