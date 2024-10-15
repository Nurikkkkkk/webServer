#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_set>

#pragma comment(lib, "Ws2_32.lib")

using namespace std::filesystem; 

#define DEFAULT_PORT "8080"
#define BUFF_LEN 1024 

class server {
    WSADATA wsaData;
    SOCKET listenSock = INVALID_SOCKET; 
    SOCKET clientSock = INVALID_SOCKET;
    struct addrinfo hints;
    struct addrinfo *addrResult = nullptr;
    std::unordered_set<std::string> fileSystem; 

    // handle request
    int requestHandler(const std::string req);
    // request parser 
    std::vector<std::string> requestParser(const std::string req);
    // add all html files 
    void loadFiles(std::unordered_set<std::string> &fileSys, path targetDirectory);

public:

    // default constructor
    server(); 
    // start server
    int startLocalServer();
};

#endif 