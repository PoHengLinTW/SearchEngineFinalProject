#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// c++ : because the header files between c and cpp are different
#include <iostream>
#include  <arpa/inet.h>
#include <sys/uio.h>


int main( int argc, char** argv ) {
    int clientfd=0;

    //              IPv4     TCP
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) { // fail
        std::cout << "Fail to create socket file descriptor" << std::endl;
        return -1;
    }

    struct sockaddr_in info;
    bzero(&info, sizeof(info));                     // initialization
    info.sin_family = PF_INET;                      // PF_INET for IPv4
    info.sin_addr.s_addr = inet_addr("127.0.0.1");  // the ip address you want to connect
    info.sin_port = htons(8080);

    int err = connect(clientfd, (struct sockaddr *)&info, sizeof(info));
    if (err == -1) {
        std::cout << "Connection error" << std::endl;
        return -2;
    }

    char recvbuf[100], input[100];
    int state = recv(clientfd, recvbuf, sizeof(recvbuf), 0);
    std::cout << "Client starts" << std::endl;
    std::cout << recvbuf << std::endl << "> ";
    std::cin >> input;
    while (1) {
        //send(clientfd, input, sizeof(input), 0);
        write(clientfd, input, sizeof(input));
        std::cout << "> ";
        std::cin >> input;
    }
    close(clientfd);
    return 0;
}
