#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT 8080
#define DEFAULT_IP "127.0.0.1"
#define BUFF_LEN 30720 

class server {
    WSADATA wsaData;
    SOCKET listenSock = INVALID_SOCKET; 
    SOCKET clientSock = INVALID_SOCKET;
    struct sockaddr_in hints;  
    int serverLen;

public:

    server(); 
    int startLocalServer();
};

#endif 