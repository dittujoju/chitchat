#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void read_sock(SOCKET sock)
{
    while (true)
    {
        char buf[4096];
        ZeroMemory(buf, 4096);
        int bytesReceived = recv(sock, buf, 4096, 0);
        if (bytesReceived > 0)
        {
            cout << string(buf, 0, bytesReceived) << endl << "> ";
        }
        else
        {
            cout << "Server disconnected" << endl;
            closesocket(sock);
            return;
        }
    }
}

void main()
{

    string ipAddress = "127.0.0.1";
    int port = 54000;

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
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR)
    {
        cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
    }

    string userName;
    cout << "Enter you Name : ";
    getline(cin, userName);

    thread t1(read_sock, sock);

    Sleep(500);
    string userInput;
    while (userInput != "exit")
    {
        
        getline(cin, userInput);
        userInput = userName + "> " + userInput;
        int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
        cout << "> ";
        if (sendResult == SOCKET_ERROR)
        {
            cout << "Send Failed!" << endl;
        }

    }
        closesocket(sock);
        WSACleanup();
}



