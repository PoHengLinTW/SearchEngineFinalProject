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

#include "sockServer.h"

sockServer::sockServer()
{
    /* varibles initialized */
    MAX_CLIENT = 10;
    MAX_MSG = 512;
    bind_state = -2;
    listen_state = -2;
    runnable = false;
    client_size = sizeof(client_info);
    on = 1;

    /* fd set initialized and set */
    FD_ZERO(&active_set);
    
    /* socket created */
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0)
        perror("socket fd");
    
    /* sockaddr_in initialized */
    bzero(&server_info, sizeof(server_info));
    
    /* set option to socket */
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    server_info.sin_family = PF_INET;
    server_info.sin_addr.s_addr = INADDR_ANY; // allow any client
    server_info.sin_port = htons(8080);

    /* bind and listen */
    bind_state = bind(serverfd, (struct sockaddr*) &server_info, sizeof(server_info));
    if (bind_state < 0)
        perror("bind");
    listen_state = listen(serverfd, MAX_CLIENT);
    if (listen_state < 0)
        perror("listen");
}

sockServer::~sockServer() {
    close(serverfd);
}

/*
 *  Check whether all the necessary elements in server is ready to go
 */
void sockServer::check() {
    runnable = true;
    if (serverfd < 0) {
        std::cerr << "X creating socketfd failed" << std::endl;
        runnable = false;
    }
    else
        std::cerr << "V creating socketfd succeeded" << std::endl;

    if (bind_state < 0) {
        std::cerr << "X binding socketfd failed" << std::endl;
        runnable = false;
    }
    else
        std::cerr << "V binding socketfd succeeded" << std::endl;

    if (listen_state < 0) {
        std::cerr << "X listen socketfd failed" << std::endl;
        runnable = false;
    }
    else
        std::cerr << "V listen socketfd succeeded" << std::endl;

    if (runnable) {
        FD_ZERO(&active_set);
        FD_SET(serverfd, &active_set);
        std::cerr << "Please use run function to start the server." << std::endl;
    }
    else
        std::cerr << "Please rerun the renewfd function to try again." << std::endl;
}

void sockServer::_check () {
    runnable = true;
    if (serverfd < 0 || bind_state < 0 || listen_state < 0)
        runnable = false;
    if (runnable) {
        FD_ZERO(&active_set);
        FD_SET(serverfd, &active_set);
    }
}

bool sockServer::renewfd() {
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    bind_state = bind(serverfd, (struct sockaddr*) &server_info, sizeof(server_info));
    listen_state = listen(serverfd, MAX_CLIENT);
    check();
    return runnable;
}

void sockServer::run() {
    _check();
    if (!runnable) {
        std::cout << "Please run check functiob to find where goes wrong" << std::endl;
        return;
    }
    std::cout << "Server starts connection" << std::endl;
    while(1) {
        read_set = active_set;
        if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
            perror("Select");
            break;
        }
        for (int fd=0; fd<FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &read_set)) { // fd in set

                if (fd == serverfd) { //server: be able to connect new client
                    int newClient = accept(serverfd, (struct sockaddr*) &client_info, &client_size);
                    if (newClient == -1) {
                        perror("Accept");
                        continue;
                    }
                    std::cerr 
                        << "Server: Client " << newClient << " connects from host(" << inet_ntoa(client_info.sin_addr) 
                        << "), port(" << ntohs(client_info.sin_port) << ")" << std::endl;
                    
                    FD_SET(newClient, &active_set);

                    // test
                    char tmp[] = "Welcome to server";
                    send(newClient, tmp, sizeof(tmp), 0);
                }
                else { //client
                    if (read_from_client(fd) < 0) {
                        std::cerr << "Client " << fd << " leaves" << std::endl;
                        close(fd);
                        FD_CLR(fd, &active_set);
                    }
                }
            }
        }
    }
}

int sockServer::read_from_client(int fd) {
    char buf[MAX_MSG];
    int nbytes = recv(fd, buf, MAX_MSG, 0);
    if (nbytes < 0) {
        perror("read_from_client: read");
        exit(EXIT_FAILURE);
    }
    else if(nbytes == 0)
        return -1;
    
    std::cerr << "Server: got message from " << fd << " : \n" << buf << std::endl;
    return nbytes;
}