#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <time.h>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#pragma comment(lib, "Ws2_32.lib")

using namespace std::filesystem; 

#define HTTP200 "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
#define HTTP404 "HTTP/1.1 404 Resource Not Found\r\nContent-Type: text/html\r\n"

#define DEFAULT_PORT "8080"
#define BUFF_LEN 1024 

class server {
    WSADATA wsaData;
    SOCKET listenSock = INVALID_SOCKET; 
    SOCKET clientSock = INVALID_SOCKET;
    struct addrinfo hints;
    struct addrinfo *addrResult = nullptr;
    std::unordered_map<std::string, std::string> fileSystem; 

    // handle request
    void requestHandler(const std::string req);
    // request parser 
    std::vector<std::string> requestParser(const std::string req);
    // send response
    void response(const std::string, int);
    // add all html files 
    void loadFiles(std::unordered_map<std::string, std::string> &fileSys, path targetDirectory);

public:

    // default constructor
    server(); 
    // start server
    int startLocalServer();
};

#endif 