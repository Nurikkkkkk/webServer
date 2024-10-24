#include "localserver.h"

server::server() {
    // Creation of SOCKET object
    ZeroMemory(&hints, sizeof(hints)); // set all fields to zero 
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // because we use TCP otherwise SOCK_DGRAM for UDP
    hints.ai_protocol = IPPROTO_TCP; // TCP 
    hints.ai_flags = AI_PASSIVE; // listening socket
    loadFiles(fileSystem, "Files\\HTML");
    std::cout << "Constructing fileSystem: " << fileSystem.size() << std::endl; 

    // //remove comments if you want to see the contents of the container 
    // for (auto el : fileSystem) {
    //     std::cout << el.first << " : " << el.second << std::endl;
    // }
}


void server::loadFiles(std::unordered_map<std::string, std::string> &fileSys, path targetDirectory) {

    // if doesn't exists -> create directory
    if (!exists(targetDirectory)) {
        create_directory(targetDirectory);
    }

    // traverse a directory 
    for (const auto& file : directory_iterator(targetDirectory)) {
        if (is_directory(file)) {
            loadFiles(fileSys, file); // recursively traverse if it is a directory
        } else {
            std::string fileName = "";
            std::string path = file.path().string();
            // get file name
            for (std::size_t i = 0; i < path.size(); i++) {
                if (path[i] == '\\') {
                    fileName = "";
                } else {
                    fileName += path[i];
                }
            }
            fileSys[fileName] = path; 
        }
    }
}

/*

--------------------------------------------------------------------------------------------------

*/

void server::response(const std::string path, int code){
    std::string serverMsg;
    std::string page = "";
    // if 404
    if (code == 404) {
        serverMsg = HTTP404;
    } else if (code == 200) {
        serverMsg = HTTP200;
    }
    if (path != "favicon.ico") {
        std::ifstream read; // open file read only 
        read.open(path);
        if (!read.is_open()) {
            serverMsg = HTTP404;
        } else {
            std::string line = "";
            while (std::getline(read, line)) {
                page.append(line+"\n");
            }
            serverMsg.append(std::to_string(page.size())); 
            serverMsg.append("\r\n\r\n");
            serverMsg.append(page);
        }
        read.close(); // close read file
    }
    // sent response 
    int bytesSent = 0, totalBytesSent = 0;
    while (totalBytesSent < serverMsg.size()) {
        bytesSent = send(clientSock, serverMsg.c_str()+totalBytesSent, serverMsg.size()-totalBytesSent, 0);
        if (bytesSent == 0) {
            std::cout << "Cannot send respond" << std::endl;
            closesocket(clientSock);
            closesocket(listenSock);
            WSACleanup();
            return;
        }
        totalBytesSent += bytesSent;
    }
    std::time_t time = std::time(nullptr); // get time 
    std::cout << "Response: " << std::asctime(std::localtime(&time)) << std::endl;
}


void server::requestHandler(const std::string req) {
    std::vector<std::string> parsed = requestParser(req);

    if (parsed.front() == "UNKNOWN" || parsed.back() == "favicon.ico") {
        response("favicon.ico", 200); // I do not have favicon.ico, therefore I ignore it 
    } else if (parsed.front() == "GET") {
        if (parsed.back() == "") {
            response(fileSystem["index.html"], 200); // index.html is a homepage  
        } else if (fileSystem.find(parsed.back()) == fileSystem.end()) {
            response(fileSystem["notFound.html"], 404); // not found 
        } else {
            response(fileSystem[parsed.back()], 200); // found
        }
    } else if (parsed.front() == "POST") {
         if (parsed.back() == "") {
            response(fileSystem["index.html"], 200); // index.html is a homepage  
        } else if (fileSystem.find(parsed.back()) == fileSystem.end()) {
            response(fileSystem["notFound.html"], 404); // not found 
        } else {
            response(fileSystem[parsed.back()], 200); // found
        }
    }
}


std::vector<std::string> server::requestParser(const std::string req) {
    std::vector<std::string> result; 

    // define request type 
    if (req.find("GET") != std::string::npos) {
        result.push_back("GET");
    } else if (req.find("POST") != std::string::npos) {
        result.push_back("POST");
    } else {
        result.push_back("UNKNOWN");
    }


    std::string fileName = "";
    bool start = false;
    // get file path 
    for (std::size_t i = 0; i < req.size(); i++) {
        if (req[i] == ' ' && !start) {
            start = true;
        } else if ((req[i] == ' ' || req[i] == '?') && start) {
            break;
        } else {
            if (req[i] == '/') {
                fileName = "";
            } else {
                fileName += req[i];
            }
        }
    }

    result.push_back(fileName);
    return result;
}

/*

--------------------------------------------------------------------------------------------------

*/

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

    std::cout << "Listening on 127.0.0.1:8080\n";
    std::cout << "Press CTRL+C to shut down the server\n\n";   

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
            
            char printRequest[32];
            std::size_t found = request.find("\n");
            if (found != std::string::npos) {
                std::size_t len = request.copy(printRequest, found);
                printRequest[len] = '\0';
            }
            std::cout << "Request: " << printRequest << std::endl;
            requestHandler(request); 
        }
        closesocket(clientSock);
    }

  
    closesocket(listenSock);
    WSACleanup();

    return 0;
}
