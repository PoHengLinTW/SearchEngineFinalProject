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

#define MAX_CLIENT 10
#define MAX_MSG 512

int read_from_client(int fd) {
    char buf[MAX_MSG];
    int nbyte = recv(fd, buf, MAX_MSG, 0);
    if (nbyte < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    else if (nbyte == 0)
        return -1;
    
    std::cerr << "Server: got message from " << fd << " : \n" << buf << std::endl;
    return nbyte;
}

int main( int argc, char** argv ) {
    int serverfd = 0, clientfd = 0, on=1;
    const char buf[] = "This is server.";
    char recvbuf[100];

    //              IPv4     TCP
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) { // fail
        perror("Socket");
        close(serverfd);
        exit(-1);
    }

    struct sockaddr_in server_info, client_info;
    socklen_t client_info_size = sizeof(client_info);
    bzero(&server_info, sizeof(server_info));       // initialization

    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

    server_info.sin_family = PF_INET;                      // PF_INET for IPv4
    server_info.sin_addr.s_addr = INADDR_ANY;
    server_info.sin_port = htons(8080);

    int bind_state = bind(serverfd, (struct sockaddr*) &server_info, sizeof(server_info));
    if (bind_state == -1 ) {
        perror("Bind");
        close(serverfd);
        exit(-2);
    }

    int listen_state = listen(serverfd, MAX_CLIENT);
    if (listen_state == -1) {
        perror("Listen");
        close(serverfd);
        exit(-3);
    }

    fd_set active_set, read_set;
    FD_ZERO(&active_set);
    FD_SET(serverfd, &active_set);

    /* start interacting with clients */
    std::cout << "Server starts" << std::endl;
    while (1) {
        read_set = active_set;
        if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
            perror("Select");
            close(serverfd);
            exit(-4);
        }
        for (int fd=0; fd<FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &read_set)) {
                if (fd == serverfd) { // server : connect new client
                    int newClient = accept(serverfd, (struct sockaddr*) &client_info, &client_info_size);
                    if (newClient == -1) {
                        perror("Accept");
                        exit(EXIT_FAILURE);
                    }
                    std::cerr 
                        << "Server: Client " << newClient << " connects from host(" << inet_ntoa(client_info.sin_addr) 
                        << "), port(" << ntohs(client_info.sin_port) << ")" << std::endl;
                    FD_SET(newClient, &active_set);
                    char tmp[] = "Welcome to server";
                    send(newClient, tmp, sizeof(tmp), 0);
                }
                else { // client
                    if (read_from_client(fd) < 0) {
                        std::cerr << "Client " << fd << " leaves" << std::endl;
                        close(fd);
                        FD_CLR(fd, &active_set);
                    }
                }
            }
        }
    }
    close(serverfd);
    return 0;
}
