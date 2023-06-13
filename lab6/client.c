#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sockop.h"

#define BUFSIZE 256

int main(int argc, char const *argv[])
{
    int connfd, n; //socket descriptor
    struct sockaddr_in addr_cln; 
    char snd[BUFSIZE];

    connfd = connectsock(argv[2], argv[1], "tcp");

    memset(snd, 0, BUFSIZE);
    sprintf(snd, "%s %s %s", argv[3], argv[4], argv[5]);
    printf("%s\n", snd);
    if((n = write(connfd, snd, strlen(snd))) == -1){
        errexit("error write %d\n", connfd);
    }

    close(connfd);    
    return 0;
}

