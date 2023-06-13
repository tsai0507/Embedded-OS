#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/sem.h>
#include <signal.h>
#include <pthread.h>

#include "sockop.h"

#define BUFSIZE 128
#define DOODLE_SEM_KEY 1010
#define SEM_MODE 0666
#define NUMTHREADS 5
#define checkResults(string, val) {                 \
        if (val) {                                          \
            printf("Failed with %d at %s", val, string);    \
            exit(1);                                        \
        }                                                   \
}

int s, server_fd;
unsigned total_money = 0;
int P(int s);
int V(int s);
void *theThread(void *parm);

void sigintHandler(int sig_num) {

    close(server_fd);
    /* remove semaphore */
    if (semctl (s, 0, IPC_RMID, 0) < 0)
    {
        fprintf (stderr, "unable to remove semaphore %d\n",
                DOODLE_SEM_KEY);
        exit(1);
    }
    printf("Semaphore %d has been remove\n", DOODLE_SEM_KEY);
    exit(0);
}

int main(int argc, char** argv){
    
    signal(SIGINT, sigintHandler);
    int rc;
    long connfd;
    struct sockaddr_in addr_cln; 
    socklen_t slen = sizeof(addr_cln);
    pthread_t *thread;
    

    s = semget(DOODLE_SEM_KEY, 1, IPC_CREAT | SEM_MODE);
    if (s < 0)
    {
        fprintf(stderr,
                    "creation of semaphore %d failed: %s\n",
                    DOODLE_SEM_KEY, strerror(errno));
        exit(1);
    }
    printf("Semaphore %d created\n", DOODLE_SEM_KEY);

    if ( semctl(s, 0, SETVAL, 1) < 0 )
    {
        fprintf(stderr,
                    "Unable to initialize semaphore: %s\n",
                    strerror(errno));
        exit(0);
    }
    printf("Semaphore %d has been initialized to %d\n", DOODLE_SEM_KEY, 1);
    
    fflush(stdin);
    server_fd = passivesock(argv[1], "tcp", 10);
    while( connfd = accept(server_fd, (struct sockaddr *)&addr_cln, (socklen_t *)&slen) )
    {   
        thread = (pthread_t*)malloc(sizeof(pthread_t));
        rc = pthread_create(thread, NULL, theThread, (void *)connfd);
        checkResults("pthread_create()\n", rc);

        rc = pthread_detach(*thread);
        checkResults("pthread_detach()\n", rc);

        free(thread); 
    }


    return 0;
}

/* P () - returns 0 if OK; -1 if there was a problem */
int P(int s)
{
    struct sembuf sop;  /* the operation parameters */
    sop.sem_num = 0;    /* access the 1st (and only) sem in the array */
    sop.sem_op = -1;    /* wait..*/
    sop.sem_flg = 0;    /* no special options needed */
    
    if (semop (s, &sop, 1) < 0) {
        fprintf(stderr,"P(): semop failed: %s\n",strerror(errno));
        return -1;
    } else {
        return 0;
    }
}

/* V() - returns 0 if OK; -1 if there was a problem */
int V(int s)
{
    struct sembuf sop;  /*the operation parameters */
    sop.sem_num = 0;    /*the 1st (and only) sem in the array */
    sop.sem_op = 1;     /* signal */
    sop.sem_flg = 0;    /* no special options needed */

    if (semop(s, &sop, 1) < 0) {
        fprintf(stderr,"V(): semop failed: %s\n",strerror(errno));
        return -1;
    } else {
        return 0;
    }
}

void *theThread(void *parm)
{
    int rc, time, amount;
    long connfd;
    char rcv[BUFSIZE], res[3][BUFSIZE/2];
    char *token;
    connfd = (long)parm;
    // connfd = *(long *)parm;

    memset(rcv, 0, sizeof(rcv));
    read(connfd, rcv, BUFSIZE);
    close(connfd); 

    token = strtok(rcv, " "); // 分割字符串
    for(int i = 0; i < 3; i++)
    {
        strcpy(res[i], token);    
        token = strtok(NULL, " ");
    }
    time = atoi(res[2]);
    amount = atoi(res[1]);
    // printf("time:%d, amount:%d\n", time, amount);
    
    
    /**************** Critical Section *****************/
    if(strstr(res[0], "deposit") != NULL){
        for(int i = 0; i < time; i++){
            P(s);
            total_money += amount;
            printf("After deposit: %d\n", total_money);
            usleep(5);
            V(s);
        }     
    }
    else if(strstr(res[0], "withdraw") != NULL){
        for(int i = 0; i < time; i++){
            P(s);
            total_money -= amount;
            printf("After withdraw: %d\n", total_money);
            usleep(5);
            V(s);
        }     
    }
    /**************** Critical Section *****************/
    

    return NULL;
}