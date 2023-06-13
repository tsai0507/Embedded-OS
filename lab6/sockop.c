#include "sockop.h"

int passivesock(const char *service, const char *transport, int qlen)
{
    struct servent *pse; //pointer to service information entry
    struct sockaddr_in sin; //Internet endpoint address
    int server_fd, type; //socket descriptor and socket type

    memset(&sin, 0, sizeof(sin));
    // 設定socket地址
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    //map service name to port number
    if((pse == getservbyname(service, transport)))
        sin.sin_port = htons(ntohs((unsigned short)pse->s_port));
    else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
        errexit("Can't find service entry %s.\n", service);
    
    //use protocol to choose a socket type
    if(strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM;
    else  
        type = SOCK_STREAM;  

    //allocate a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
        errexit("Can't allocate a socket %s.\n", service);
    
    //bind the socket
    if (bind(server_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        errexit("Can't bind the socket %s.\n", service);
    
    //set the maxmum number of waiting connect
    if(type == SOCK_STREAM && listen(server_fd, qlen))
        errexit("Can't listen on port %s.\n", service);

    return server_fd;
}

int connectsock(const char *host, const char *service_IP, const char *transport)
{
    struct hostent *phe; //pointer to host information entry
    struct servent *pse; //pointer to service information entry
    struct sockaddr_in sin; //Internet endpoint address
    int client_fd, type; //socket descriptor and socket type

    memset(&sin, 0, sizeof(sin));
    // 設定socket地址
    sin.sin_family = AF_INET;

    //map service name to port number
    if((pse == getservbyname(host, transport))){
        printf("yes we get\n");
        sin.sin_port = pse->s_port;
    }
    else if((sin.sin_port = htons((unsigned short)atoi(host))) == 0)
        errexit("Can't find service entry %s.\n", host);
    
    if (inet_pton(AF_INET, service_IP, &sin.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    //use protocol to choose a socket type
    if(strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM;
    else  
        type = SOCK_STREAM;  
    
    //allocate a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0)
        errexit("Can't allocate a socket %s.\n", host);

    //connet the socket
    if((connect(client_fd, (struct sockaddr *)&sin, sizeof(sin))) < 0)
        errexit("Can't connect to %s.\n", host);

    return client_fd;
}