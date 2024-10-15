#include "localserver.h"

//-lwsock32 -lWs2_32  for compiling

int main(){

    server server; 
    int status = server.startLocalServer();
    if (status != 0) {
        std::cout << "Shutting down...";
        return 1;
    }

    return 0;
}