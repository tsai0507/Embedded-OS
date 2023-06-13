#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "sockop.h"

#define BUFSIZE 1024
#define TYPE 9

typedef struct{
    char c[128][128];
    int i;
    bool n;
}message;

message analyzeStr(char* str);
int CheckDis(char* str);
int* exTypeNum(char* str);

/*Create to save data of area number*/ 

// unsigned dist_data[9][4] = {0};
int dist_data[9][4] = {0};

unsigned TOTAL = 0;
char* dist_name[TYPE] = {"Baseball Stadium", "Big City", "Neiwan Old Street",
                        "NYCU", "Smangus", "17 km of Splendid Coastline",
                        "100 Tastes", "Science Park", "City God Temple"};
char* PeoType[3] = {"Child", "Adult", "Elder"};

int main(int argc, char** argv){

    char* MODE;
    short* led;
    int server_fd, connfd, seven;
    struct sockaddr_in addr_cln; 
    socklen_t slen = sizeof(addr_cln);
    char snd[BUFSIZE], rcv[BUFSIZE], usbuf[BUFSIZE];
    message msg;
    char *pos;

    memset(snd, 0, BUFSIZE);
    memset(rcv, 0, BUFSIZE);
    memset(usbuf, 0, BUFSIZE);

    fflush(stdout);
    server_fd = passivesock(argv[1], "tcp", 10);

    while(connfd = accept(server_fd, (struct sockaddr *)&addr_cln, (socklen_t *)&slen))
    {
        while(1){
            memset(rcv, 0, sizeof(rcv));
            read(connfd, rcv, BUFSIZE);
            printf("%s\n", rcv);
            msg = analyzeStr(rcv);
            fflush(stdout);
            if((pos = strstr(msg.c[0], "Menu")) != NULL)
            {
                memset(snd, 0, sizeof(snd));
                sprintf(snd, "1. Search\n2. Report\n3. Exit\n");
                write(connfd, snd, strlen(snd)+1);
            }

            else if((pos = strstr(msg.c[0], "Exit")) != NULL)
            {
                memset(dist_data, 0, sizeof(dist_data));
                close(connfd);
                break;
            }

            else if((pos = strstr(msg.c[0], "Search")) != NULL)
            {
                if(msg.i == 1)
                {
                    memset(snd, 0, sizeof(snd));
                    for(int i = 0; i < TYPE; i++)
                    { 
                        sprintf(usbuf, "%d. %s : %d\n",(i + 1) , dist_name[i], dist_data[i][3]);
                        strcat(snd, usbuf);
                    }
                    write(connfd, snd, strlen(snd)+1);
                }
                else
                {
                    int dist;
                    dist = CheckDis(msg.c[1]);
                    memset(snd, 0, sizeof(snd));
                    sprintf(snd, "%s - Child : %d Adult : %d Elder : %d\n",
                                dist_name[dist], dist_data[dist][0], dist_data[dist][1], dist_data[dist][2]);
                    write(connfd, snd, strlen(snd)+1);
                }
            }

            else if((pos = strstr(msg.c[0], "Report")) != NULL)
            {
                int wait, dist;
                int *IndexNum;
                char* waitInfo = "Please wait a few seconds...\n";
                memset(snd, 0, BUFSIZE);
                
                for(int i = 1; i < msg.i; i+=2)
                {
                    dist = CheckDis(msg.c[i]);
                    IndexNum = exTypeNum(msg.c[i+1]);
                    if(IndexNum[0] < 0 || IndexNum[0] >= 3){
                        printf("Faill\n");
                        break;
                    }
                    if(dist > wait) wait = dist + 1;
                    dist_data[dist][IndexNum[0]] += IndexNum[1];
                    dist_data[dist][3] += IndexNum[1];
                    memset(usbuf, 0, sizeof(usbuf));
                    fflush(stdout);
                    sprintf(usbuf, "%s | %s %d\n",dist_name[dist] , PeoType[IndexNum[0]], IndexNum[1]);
                    strcat(snd, usbuf);
                    // printf("%s",snd);
                }

                write(connfd, waitInfo, strlen(waitInfo)+1);
                if(msg.i == 3) 
                    sleep(1);
                else{
                    sleep(wait);
                } 
                // printf("%s",snd);
                // printf("%ld",strlen(snd));
                write(connfd, snd, strlen(snd)+1);
            }
        }
    }
    close(server_fd);

    return 0;
}

message analyzeStr(char* str){

    message msg;
    int i = 0;
    char *token;
    // printf("%s\n", str);
    token = strtok(str, "|" ); // 分割字符串
    while (token != NULL) {
        // printf("%s\n", token);
        strcpy(msg.c[i], token);    
        i++;
        token = strtok(NULL, "|");
    }
    msg.i = i;
    msg.n = true;

    return msg;
}

int CheckDis(char* str){
    char *pos;
    if((pos = strstr(str, "Baseball Stadium")) != NULL) return 0;
    else if((pos = strstr(str, "Big City")) != NULL) return 1;
    else if((pos = strstr(str, "Neiwan Old Street")) != NULL) return 2;
    else if((pos = strstr(str, "NYCU")) != NULL) return 3;
    else if((pos = strstr(str, "Smangus")) != NULL) return 4;
    else if((pos = strstr(str, "17 km of Splendid Coastline")) != NULL) return 5;
    else if((pos = strstr(str, "100 Tastes")) != NULL) return 6;
    else if((pos = strstr(str, "Science Park")) != NULL) return 7;
    else if((pos = strstr(str, "City God Temple")) != NULL) return 8;
    else return -1;
}

int* exTypeNum(char* str){

    int* IndexNum = malloc(sizeof(int) * 2);
    char *token;

    token = strtok(str, " " ); // 分割字符串
    if(strcmp(token, PeoType[0]) == 0) IndexNum[0] = 0;
    else if(strcmp(token, PeoType[1]) == 0) IndexNum[0] = 1;
    else if(strcmp(token, PeoType[2]) == 0) IndexNum[0] = 2;   

    token = strtok(NULL, " ");
    IndexNum[1] = atoi(token);

    // token = strtok(NULL, " ");
    // if(token != NULL) return NULL;
    // else 
    return IndexNum;
}

