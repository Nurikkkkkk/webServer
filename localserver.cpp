#include "localserver.h"

server::server() {
    // Creation of SOCKET object
    ZeroMemory(&hints, sizeof(hints)); // set all fields to zero 
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // because we use TCP otherwise SOCK_DGRAM for UDP
    hints.ai_protocol = IPPROTO_TCP; // TCP 
    hints.ai_flags = AI_PASSIVE; 
}

int server::startLocalServer() {
    std::cout << "Initializing server..." << std::endl;

    // winsock initialization 
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup fail" << std::endl;
        return 1;
    }

    // define IP and port 
    struct addrinfo *addrResult = nullptr; // pointer to a linked list of addrinfo structs
    if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &addrResult) != 0) {
        std::cout << "Getaddrinfo fail" << std::endl;
        WSACleanup();
        return 1;
    }

    // create socket
    listenSock = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (listenSock == INVALID_SOCKET) {
        std::cout << "Create socket failed" << std::endl;
        return 1;
    }

    // binding socket
    if (bind(listenSock, addrResult->ai_addr, addrResult->ai_addrlen) == SOCKET_ERROR) {
        std::cout << "Socket bind error" << std::endl;
        freeaddrinfo(addrResult); 
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(addrResult); // no longer needed 

    // listen for connections 
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed" << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    } 

    std::cout << "Listening on 127.0.0.1:8080" << std::endl;
    std::cout << "Press CTRL+C to shut down the server" << std::endl;   

    int bytes = 0;
    while(true) { 
         // accept client
        clientSock = accept(listenSock, NULL, NULL);
        if (clientSock == INVALID_SOCKET) {
            std::cout << "Cannot accept" << std::endl;
        }

        char recvBuff[BUFF_LEN];
        bytes = recv(clientSock, recvBuff, BUFF_LEN, 0);
        if (bytes == SOCKET_ERROR) {
            std::cout << "Cannot read the request" << std::endl;
            closesocket(clientSock);
            closesocket(listenSock);
            WSACleanup();
            return 1;
        } else {
            recvBuff[bytes] = '\0';
            std::string request(recvBuff);
            std::cout << "Received request:\n" << request << std::endl << requestHandler(request) << std::endl;
        }

        

        std::string serverMsg = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        std::string resp = "<html><h1>Hello</h1></html>";
        serverMsg.append(std::to_string(resp.size()));
        serverMsg.append("\n\n");
        serverMsg.append(resp);

        std::cout <<serverMsg << std::endl;

        int bytesSent = 0, totalBytesSent = 0;
        while (totalBytesSent < serverMsg.size()) {
            bytesSent = send(clientSock, serverMsg.c_str()+totalBytesSent, serverMsg.size()-totalBytesSent, 0);
            if (bytesSent == 0) {
                std::cout << "Cannot send respond" << std::endl;
                closesocket(clientSock);
                closesocket(listenSock);
                WSACleanup();
                return 1;
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

int server::requestHandler(const std::string req) {
    std::vector<std::string> parsed = requestParser(req);
    for (auto el : parsed) {
        std::cout << el << std::endl;
    }
    return 0;
}

std::vector<std::string> server::requestParser(const std::string req){
    std::vector<std::string> result; 

    // define request type 
    if (req.find("GET") == 0) {
        result.push_back("GET");
    } else if (req.find("POST" == 0)) {
        result.push_back("POST");
    } else {
        result.push_back("UNKNOWN");
    }

    std::string path = "";
    bool start = false;
    // get file path 
    for (size_t i = 0; i < req.size(); i++) {
        if (req[i] == ' ' && !start) {
            start = true;
        } else if (req[i] == ' ' && start) {
            break;
        } else {
            path += req[i];
        }
    }

    std::string fileName = "";
    // get file name
    for (size_t i = 0; i < path.size(); i++) {
        if (path[i] == '/') {
            fileName = "";
        } else {
            fileName += path[i];
        }
    }
    result.push_back(fileName);
    return result;
}