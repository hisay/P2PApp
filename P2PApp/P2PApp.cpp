// P2PApp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "CP2PServerListener.h"
#include "CP2PClient.h"
#include <thread>

#define S_PORT  4142

bool InitNet()
{
    return SocketInit::Init();
    //WORD w = MAKEWORD(2, 2);
    //WSAData d;
    //return 0 ==WSAStartup(w, &d);
}

 
void Server()
{
    CP2PServerListener listener,listener2;
    listener2.SetP2PLink(&listener);
    if (listener.Listener(S_PORT) && listener2.Listener(S_PORT+1))
    {
        cout << "Listening on port:" << S_PORT <<"-"<<(S_PORT+1)<< endl;
        while (true) {
            if (!listener.Update() || !listener2.Update()) {
                cout << "Listener update failed,exit server !" << endl;
                break;
            }
            else {
                this_thread::sleep_for(std::chrono::nanoseconds{ 1 });
            }
        }
    }
   /* SOCKET  skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (INVALID_SOCKET == skt) { cout << "InitSocket failed(" << WSAGetLastError() << ")" << endl; return; }

    do {

        SOCKADDR_IN addr = { 0 };
        addr.sin_addr.S_un.S_addr = INADDR_ANY;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(S_PORT);

        if (bind(skt, (sockaddr*)&addr, sizeof(addr))) {
            cout << "Bind socket failed:" << WSAGetLastError() << endl;
            break;
        }

        if(listen(skt,5))
        {
            cout << "Listen socket failed:" << WSAGetLastError() << endl;
            break;
        }

        while (true) {
            SOCKADDR_IN caddr = { 0 };
            int caddrlen = sizeof(caddr);
            SOCKET m = accept(skt, (sockaddr*)&caddr, &caddrlen);
            if (INVALID_SOCKET == m) {
                cout<<"Accept socket failed!"<<WSAGetLastError() << endl;
                break;
            }
            struct SParam
            {
                SOCKADDR_IN addr;
                SOCKET skt;
            };
 
            new thread([](SParam  p)->void {
                SOCKADDR_IN ca = p.addr;
                SOCKET cskt = p.skt;
                BYTE    buf[50] = { 0 };
                int n;
                do {
                    n = recv(cskt, (char*)buf, 50, 0);
                    if (n >= sizeof(P_NHeader))
                    {
                        P_NHeader* pNH = (P_NHeader*)buf;
                        if (!CheckPackage(pNH))break;
                        switch (pNH->type)
                        {

                        case CONN_LOGIN:
                        {

                            P_SelfAddr sa;
                            sa.header.flag = PH_HEADER;
                            sa.header.len = sizeof(sa);
                            sa.header.type = SELF_ADDR;
                            sa.addr = ca.sin_addr.S_un.S_addr;
                            sa.port = ntohs(ca.sin_port);
                            n = send(cskt, (const char*)&sa, sizeof(sa), 0);
                            if (n != sizeof(sa))
                            {
                                cout << "Send self addr failed:" << WSAGetLastError() << endl;
                                break;
                            }
                        }
                        break;
                        }

 
                    }
                    else {
                        cout << "recv first package failed!" << WSAGetLastError() << endl;
                    }
                } while (0);
                closesocket(cskt);
                },
            (SParam{ addr=caddr,skt=m}));
        }

    } while (false);
    closesocket(skt);*/
}

void Client(const char* ip, int port)
{
    CP2PClient clt;
    {
        if (!clt.Connect(ip, port)) {
            cout << "failed to connect " << ip << ":" << port << endl;
            return;
        }
        string msg;
        //UINT cmd = CP2PClient::EM_CONNSVR;
        //msg.append((const char*)&cmd, sizeof(cmd));
        //clt.SendMessage(msg);

        auto my = std::thread([&] {
            while (true) {
                if (!clt.Update())
                    break;
                else {
                    std::this_thread::sleep_for(std::chrono::nanoseconds{ 1 });
                }
            }
        });
        do {
            cout << ":>";
            char buf[512] = { 0 };
            std::getline(std::cin, msg);

            if (msg == "exit") {
                cout << "bye" << endl;

                UINT cmd = CP2PClient::EM_EXITAPP;
                msg = string((const char*)&cmd, sizeof(cmd));
                clt.SendMessage(msg);

                break;
            }
            else if (msg == "query") {
                UINT cmd = CP2PClient::EM_QUERY_ID;
                msg = string((const char*)&cmd, sizeof(cmd));
                clt.SendMessage(msg);
            }
            else if (msg.starts_with("conn "))
            {
                DWORD d = atol(msg.substr(5).c_str());

                UINT cmd = CP2PClient::EM_CONN_ID;
                msg = string((const char*)&cmd, sizeof(cmd));
                msg.append((const char*)&d, sizeof(d));
                clt.SendMessage(msg);
            }
            else if (msg.starts_with("send "))
            {
                UINT cmd = CP2PClient::EM_SENDMSG;
                string txt = msg.substr(5);
                UINT d = (UINT)txt.size();
                msg = string();
                msg.append((const char*)&cmd, sizeof(cmd));
                msg.append((const char*)&d, sizeof(d));
                
                msg.append((const char*)txt.c_str(), txt.size());
                clt.SendMessage(msg);
            }
        } while (true);
        my.join();
    }
}

int main(int argc,char* argv[])
{
    InitNet();

    if (argc == 2 && _stricmp(argv[1], "s") == 0)
    {
        Server();
    }
    else if (argc == 4 && _stricmp(argv[1], "c") == 0) {
        Client(argv[2], atoi(argv[3]));
    }

    return 0;
}

