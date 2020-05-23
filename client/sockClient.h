#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// c++ : because the header files between c and cpp are different
#include <iostream>
#include <arpa/inet.h>
#include <sys/uio.h>

class sockClient
{
private:
    int MAX_MSG;
    int clientfd;
    int connect_state;
    struct sockaddr_in client_info;
    bool runnable;
    char recvbuf[512];
    char sendbuf[512];
    void _check();
public:
    sockClient();
    ~sockClient();
    void check();
    void run();
};

sockClient::sockClient()
{
    MAX_MSG = 512;

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1)
        perror("socket");

    bzero(&client_info, sizeof(client_info));
    client_info.sin_family = PF_INET;
    client_info.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_info.sin_port = htons(8080);

    connect_state = connect(clientfd, (struct sockaddr *)&client_info, sizeof(client_info));
    if (connect_state == -1)
        perror("connect");
}

sockClient::~sockClient()
{
}

void sockClient::check() {
    runnable = true;
    if (clientfd == -1) {
        std::cerr << "X creating socketfd failed" << std::endl;
        runnable = false;
    }
    else
        std::cerr << "V creating socketfd succeed" << std::endl;

    if (connect_state == -1) {
        std::cerr << "X fail to connect to server" << std::endl;
        runnable = false;
    }
    else
        std::cerr << "V connect to server successfully" << std::endl;
    
    if (runnable)
        std::cerr << "Please use \"send2server\" and \"revcfromserver\" to communicate with server." << std::endl;
    else
        std::cerr << "Please rerun the class to try again." << std::endl;
    
}

void sockClient::_check() {
    if (clientfd == -1 || connect_state == -1) {
        runnable = false;
    }
    else
        runnable = true;
}


/*bool sockClient::send2server(std::string str){
    if (!runnable){
        std::cerr << "Please check where goes wrong" << std::endl;
        return false;
    }
    if (write(clientfd, str.c_str(), sizeof(str)) < 0)
        return false;
    return true;
}

int sockClient::revcfromserver() {
    if (!runnable){
        std::cerr << "Please check where goes wrong" << std::endl;
        return -1;
    }
    memset(recvbuf, '\0', sizeof(recvbuf));
    return recv(clientfd, recvbuf, sizeof(recvbuf), 0);
}*/

void sockClient::run() {
    _check();
    int nbytes;
    if (!runnable){
        std::cerr << "Please check where goes wrong" << std::endl;
        return;
    }
    recv(clientfd, recvbuf, sizeof(recvbuf), 0);
    std::cout << recvbuf << std::endl;
    while(1) {
        std::cout << "> ";
        std::cin.getline(sendbuf, sizeof(sendbuf));
        if (strncmp(sendbuf, "quit", 4)==0)
            break;
        send(clientfd, sendbuf, sizeof(sendbuf), 0);
    }
}