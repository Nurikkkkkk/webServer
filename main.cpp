#include "localserver.h"

int main(){

    server server; 
    int status = server.startLocalServer();
    if (status != 0) {
        std::cout << "Shutting down...";
        return 1;
    }

    return 0;
}