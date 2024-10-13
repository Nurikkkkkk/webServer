#include "localserver.h"

server::server() {
    // setting up socket information 
    hints.sin_family = AF_INET;
    hints.sin_addr.s_addr = inet_addr(DEFAULT_IP);
    hints.sin_port = htons(DEFAULT_PORT); 
    serverLen = sizeof(hints);
}

int server::startLocalServer() {
    std::cout << "Initializing server..." << std::endl;

    // winsock initialization 
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup fail" << std::endl;
        return 1;
    }

    // create socket
    listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cout << "Create socket failed" << std::endl;
        return 1;
    }

    // binding socket
    if (bind(listenSock, (SOCKADDR *)&hints, serverLen) == SOCKET_ERROR) {
        std::cout << "Socket bind error" << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    // listen for connections 
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed" << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    } 

    std::cout << "Listening on 127.0.0.1:8080" << std::endl;

    int bytes = 0;
    while(true) {
        clientSock = accept(listenSock, (SOCKADDR*)&hints, &serverLen);
        if (clientSock == INVALID_SOCKET) {
            std::cout << "Cannot accept" << std::endl;
        }

        char recvBuff[BUFF_LEN];
        bytes = recv(clientSock, recvBuff, (int)strlen(recvBuff), 0);
        if (bytes < 0) {
            std::cout << "Cannot read the request" << std::endl;
        }
        
        std::string serverMsg = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        std::string resp = "<html><h1>Hello World<h1></html>";
        serverMsg.append(std::to_string(resp.size()));
        serverMsg.append("\n\n");
        serverMsg.append(resp);

        int bytesSent = 0, totalBytesSent = 0;
        while (totalBytesSent < serverMsg.size()) {
            bytesSent = send(clientSock, serverMsg.c_str(), serverMsg.size(), 0);
            if (bytesSent == 0) {
                std::cout << "Cannot send respond" << std::endl;
            }
            totalBytesSent += bytesSent;
        }
        std::cout << "Sent respons to client" << std::endl;

        closesocket(clientSock);
    }

    closesocket(listenSock);
    WSACleanup();

    return 0;
}