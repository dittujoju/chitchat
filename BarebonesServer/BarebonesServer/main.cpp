#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
    // Intialize winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        cerr << "Can't Initialize winsock! Quitting" << endl;
        return;
    }

    // Create a socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return;
    }
    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // could aslo use inet_pton..
    
    bind(listening, (sockaddr*)&hint, sizeof(hint));

    // Tell winsock the socket is for listening
    listen(listening, SOMAXCONN);

    fd_set master;
    FD_ZERO(&master);

    FD_SET(listening, &master);

    while (true)
    {
        fd_set copy = master;

        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

        for (int i = 0; i < socketCount; i++)
        { 
            SOCKET sock = copy.fd_array[i];
            if (sock == listening)
            {
                SOCKET client = accept(listening, nullptr, nullptr);

                FD_SET(client, &master);

                string welcomeMsg = "Welcome to the Awesome Chat Server!";
                send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
            }
            else
            {   
                char buf[4096];
                ZeroMemory(buf, 4096);
                int bytesIn = recv(sock, buf, 4096, 0);
                if (bytesIn <= 0)
                { 
                    closesocket(sock);
                    FD_CLR(sock, &master); 
                }
                else
                {
                    for (int i = 0; i < master.fd_count; i++)
                    {
                        SOCKET outSock = master.fd_array[i];
                        if (outSock != listening && outSock != sock)
                        {
                            send(outSock, buf, bytesIn, 0);
                        }
                    }
                }

            }
        }
    }



    // cleanup winsock
    WSACleanup();
}