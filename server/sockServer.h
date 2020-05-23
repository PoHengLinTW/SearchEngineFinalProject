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


class sockServer
{
private:
    int MAX_CLIENT, MAX_MSG;
    int serverfd, clientfd;
    int bind_state, listen_state;
    int on;
    struct sockaddr_in server_info, client_info;
    socklen_t client_size;
    fd_set active_set, read_set;
    bool runnable;
    int read_from_client(int);
    void _check();
public:
    sockServer();
    ~sockServer();
    void check();
    void run();
    bool renewfd();
};
