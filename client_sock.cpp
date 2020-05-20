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
    int sockfd=0;

    /*
     *  Create socket file descriptor
     *  Prototype :
     *      fd = socket(int domain, int type, int protocal)
     * 
     *  Argument : 
     *      Domain: the field socket should make communication
     *      - AF_UNIX / AF_LOCAL  : local transmition of process and process, 
     *                              making two processes share a common file system
     *      - AF_INET  : servers and clients connected through internet (IPv4 protocal)
     *      - AF_INET6 : have same function as AF_INET but used with IPv6 protocal
     * 
     *      Type: the way of transmition
     *      - SOCK_STREAM   : provide a sequential flow with bytes as unit (TCP)
     *      - SOCK_DGRAM    : provide flow with datagram, package (UDP)
     * 
     *      Protocal :
     *      - 0     : let kernel choose the default protocal with type you pick
     * 
     *  Return value :
     *      if succeed, return socket file descriptor which we can manipulate later
     *      if fail, return -1 (INVALID_SOCKET)
     */

    //              IPv4     TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) { // fail
        std::cout << "Fail to create socket file descriptor" << std::endl;
        return -1;
    }

    /* 
     *  In <netinet/in.h>
     *  struct sockaddr_in {
     *      short           sin_family;     // PF_INET for IPv4 or PF_INET6 for IPv6
     *      unsigned short  sin_port;       // store the port number
     *      struct in_addr  sin_addr;       // 
     *      char            sin_zero[8];    // Not used, must be zero
     *  };
     */
    struct sockaddr_in info;
    bzero(&info, sizeof(info));                     // initialization
    info.sin_family = PF_INET;                      // PF_INET for IPv4

    /*
     *  inet_addr: turn the string format of address into binary data format
     *  e.g. 210.25.132.181 (ASCII string, IPv4, numbers-and-dots notation)
     *      -> 3524887733
     */
    info.sin_addr.s_addr = inet_addr("127.0.0.1");  // the ip address you want to connect
    
    /*
     *  htons: Host To Network Short integer
     *      use to deal with the problem of different "endian" in different places
     *  other family:
     *      - htonl (Host to Network Long integer)
     *      - ntohl (Network to Host Long integer)
     *      - ntohs (Network to Host Short integer)
     */
    info.sin_port = htons(8080);

    /*
     *  Connect to server
     *  Prototype :
     *      int connect(int fd, struct sockaddr *server, int addr_len);
     * 
     *  Argument :
     *      - fd        : the socket file descriptor which has been created
     *      - server    : the infomation of the server
     *      - addr_len  : size of server
     *  Return value : 
     *      if succeed, return 0
     *      else return -1
     */
    int err = connect(sockfd, (struct sockaddr *)&info, sizeof(info));
    if (err == -1) {
        std::cout << "Connection error" << std::endl;
        return -2;
    }

    /*
     *  Receive data from server
     *  Prototype :
     *      ssize_t recv(int fd, void *buf, size_t len, int flags);
     * 
     *  Argument :
     *      - fd    : socket file descriptor
     *      - buf   : the buffer where socket will store the data when receiving them
     *      - len   : size of the buffer
     *      - flags : normally is 0 unless you want to set something special
     * 
     *  Return value :
     *      - bigger than 0 : the size of data received     
     *      - equal to 0    : 
     *              1. the data size is 0
     *              2. different setting domain, e.g. one is network and the other is local
     *              3. end the connection and return 0 (end-of-file)
     *      - -1            : error occurs when receiving data
     */
    char recvbuf[100], input[100];
    int state = recv(sockfd, recvbuf, sizeof(recvbuf), 0);
    std::cout << "Client starts" << std::endl;
    std::cout << recvbuf << std::endl << "> ";
    std::cin >> input;
    while (1) {
        //send(sockfd, input, sizeof(input), 0);
        write(sockfd, input, sizeof(input));
        std::cout << "> ";
        std::cin >> input;
    }
    close(sockfd);
    return 0;
}
