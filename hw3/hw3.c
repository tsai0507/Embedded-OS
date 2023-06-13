#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //dup2
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>
#define BUFSIZE 512
#define TYPE 9
#define DOODLE_SEM_KEY 1010
#define SEM_MODE 0666
#define SEM_NUMBER 14

typedef struct
{
    char c[128][128];
    int i;
} message;
typedef struct
{
    unsigned data[9][4];
    bool Type[9];
    int len;
} refresh_unit;
typedef struct
{
    int num;
    int time;
} onBUS;

int CheckDis(char *str);
void *theThread(void *parm);
int P(int s, int index);
int V(int s, int index);
void sigintHandler(int sig_num);
void refresh_data(refresh_unit unit);
int ifrepet(refresh_unit unit);
void refresh_bustime(int wait, int bus_index);
void socket_connetion(char* port);
unsigned dist_data[9][4] = {0};
const char *dist_name[TYPE] = {"Baseball Stadium", "Big City", "Neiwan Old Street",
                               "NYCU", "Smangus", "17 km of Splendid Coastline",
                               "100 Tastes", "Science Park", "City God Temple"};
const char *PeoType[3] = {"Child", "Adult", "Elder"};
long server_fd;
int s;
onBUS bus[2];
bool Onwait[9] = {0};


int main(int argc, char **argv)
{
    signal(SIGINT, sigintHandler);
    struct sockaddr_in addr_cln;
    socklen_t slen = sizeof(addr_cln);
    // pthread_t thread[50];
    pthread_t* thread;
    int index = 0;
    long connfd, Uconnfd;
    bus[0].time = 0;bus[1].time = 0;

    // create semaphore
    s = semget(DOODLE_SEM_KEY, SEM_NUMBER, IPC_CREAT | SEM_MODE);
    for (int i = 0; i < SEM_NUMBER; i++)
        semctl(s, i, SETVAL, 1);
    printf("Semaphore %d created\n", DOODLE_SEM_KEY);

    socket_connetion(argv[1]);
    while (Uconnfd = accept(server_fd, (struct sockaddr *)&addr_cln, (socklen_t *)&slen))
    {
        thread = (pthread_t *)malloc(sizeof(pthread_t));
        connfd = Uconnfd;
        pthread_create(thread, NULL, theThread, (void *)connfd);
        pthread_detach(*thread);
        free(thread);
    }
    close(server_fd);

    return 0;
}

void sigintHandler(int sig_num)
{
    int total = 0, child = 0, adult = 0, elder = 0;
    close(server_fd);
    for(int i=0;i<9;i++){
        total += dist_data[i][3];
        child += dist_data[i][0];
        adult += dist_data[i][1];
        elder += dist_data[i][2];
    }
    /* remove semaphore */
    semctl(s, 0, IPC_RMID, 0);
    printf("Semaphore %d has been remove\n", DOODLE_SEM_KEY);
     /* write into txt file */
    FILE* fp;
    fp = fopen("result.txt", "w+");
    fprintf(fp, "Total : %d\n", total);
    fprintf(fp, "Child : %d\n", child);
    fprintf(fp, "Adult : %d\n", adult);
    fprintf(fp, "Elder : %d", elder);

	fclose(fp);
    exit(0);
}

/* P () - returns 0 if OK; -1 if there was a problem */
int P(int s, int index)
{
    struct sembuf sop;   /* the operation parameters */
    sop.sem_num = index; /* access the 1st (and only) sem in the array */
    sop.sem_op = -1;     /* wait..*/
    sop.sem_flg = 0;     /* no special options needed */
    semop(s, &sop, 1);
    return 0;
}

/* V() - returns 0 if OK; -1 if there was a problem */
int V(int s, int index)
{
    struct sembuf sop;   /*the operation parameters */
    sop.sem_num = index; /*the 1st (and only) sem in the array */
    sop.sem_op = 1;      /* signal */
    sop.sem_flg = 0;     /* no special options needed */
    semop(s, &sop, 1);
    return 0;
}

void *theThread(void *parm)
{
    char snd[BUFSIZE], rcv[BUFSIZE], usbuf[BUFSIZE];
    long connfd;
    message msg;
    int count = 0;
    char* usercv = rcv, *human, *pos, *p;

    connfd = (long)parm;
    memset(snd, 0, BUFSIZE);
    memset(rcv, 0, BUFSIZE);
    memset(usbuf, 0, BUFSIZE);

    int time = 0;
    while (1)
    {
        memset(rcv, 0, BUFSIZE);
        memset(snd, 0, sizeof(snd));
        memset(msg.c, 0, sizeof(msg.c));
        msg.i = 0;count = 0;
        read(connfd, rcv, BUFSIZE);
        fflush(stdin);

        usercv = rcv;
        while ((p = strtok_r(usercv, "|", &usercv))) {
            strcpy(msg.c[count++], p);
            ++msg.i;
        }
        count = 0;

        if ((pos = strstr(msg.c[0], "Menu")) != NULL)
        {
            sprintf(snd, "1. Search\n2. Report\n3. Exit\n");
            write(connfd, snd, strlen(snd) + 1);
        }
        else if ((pos = strstr(msg.c[0], "Exit")) != NULL)
        {
            close(connfd);
            break;
        }
        else if ((pos = strstr(msg.c[0], "Search")) != NULL)
        {
            if (msg.i == 1)
            {
                for (int i = 0; i < TYPE; i++)
                {
                    sprintf(usbuf, "%d. %s : %d\n", (i + 1), dist_name[i], dist_data[i][3]);
                    strcat(snd, usbuf);
                }
                write(connfd, snd, strlen(snd) + 1);
            }
            else
            {
                int dist;
                dist = CheckDis(msg.c[1]);
                sprintf(snd, "%s - Child : %d | Adult : %d | Elder : %d\n",
                        dist_name[dist], dist_data[dist][0], dist_data[dist][1], dist_data[dist][2]);
                write(connfd, snd, strlen(snd) + 1);
            }
        }
        else if ((pos = strstr(msg.c[0], "Report")) != NULL)
        {
            int wait = 0, dist = -1, i_add, human_index;
            char *waitInfo = "The shuttle bus is on it's way...\n";
            refresh_unit repo;
            memset(repo.data, 0, sizeof(repo.data));
            memset(repo.Type, 0, sizeof(repo.Type));

            do{
                count++;
                if((strstr(msg.c[count], "Child") == NULL) && 
                    (strstr(msg.c[count], "Adult") == NULL) && (strstr(msg.c[count], "Elder") == NULL))
                {
                    dist = CheckDis(msg.c[count]);
                    if ((dist + 1) > wait) wait = dist + 1;
                    repo.Type[dist] = 1;
                }
                else
                {
                    if(strstr(msg.c[count], "Child") != NULL) human_index = 0;
                    else if(strstr(msg.c[count], "Adult") != NULL) human_index = 1;
                    else if(strstr(msg.c[count], "Elder") != NULL) human_index = 2;
                    human = msg.c[count];
                    p = strtok_r(human, " ", &human);
                    p = strtok_r(human, " ", &human);
                    int num_temp = atoi(p);
                    repo.data[dist][human_index] += num_temp;
                    repo.data[dist][3] += num_temp;
                }
            }while((strlen(msg.c[count]) != 0) && (strlen(msg.c[count+1]) != 0));

            // deal with the output data //
            memset(usbuf, 0, sizeof(usbuf));
            for(int i = 0; i < 9; i++){
                if(repo.Type[i] == 1)
                {
                    sprintf(usbuf, "%s", dist_name[i]);
                    strcat(snd, usbuf);
                    for(int j = 0; j < 3; j++){
                        if(repo.data[i][j] > 0)
                        {
                            sprintf(usbuf, " | %s %d", PeoType[j], repo.data[i][j]);
                            strcat(snd, usbuf);
                        }
                    }
                    strcat(snd, "\n");
                }
            }
            // deal with bus problem//
            write(connfd, waitInfo, strlen(waitInfo) + 1);
            
            int repet = ifrepet(repo);
            if(repet != -1)
            {
                P(s, repet);V(s, repet);
            }
            else 
            {
                ////// lock waiting list //////
                for (int i = 0; i < 9; i++){
                    if((repo.Type[i] == 1) && (Onwait[i] == 0)){
                        Onwait[i] = 1;
                        P(s, i); 
                    }     
                }
                ////// lock waiting list //////

                
                if (bus[0].time <= bus[1].time){
                    refresh_bustime(wait, 0);
                    P(s, 9); //bus 1
                    ////// critical section for bus1 //////
                    sleep(wait);
                    refresh_bustime((-1)*wait, 0);
                    ////// critical section for bus1 //////
                    V(s, 9);
                }
                else{
                    refresh_bustime(wait, 1);
                    P(s, 11);    //bus 2
                    ////// critical section for bus2 //////
                    sleep(wait);
                    refresh_bustime((-1)*wait, 1);
                    ////// critical section for bus2 //////
                    V(s, 11);
                } 

                ////// unlock waiting list ///////
                for (int i = 0; i < 9; i++){
                    if((repo.Type[i] == 1) && (Onwait[i] == 1)){
                        Onwait[i] = 0;
                        V(s, i);
                    }    
                }
                ////// unlock waiting list //////
            }
            fflush(stdout);
            refresh_data(repo);
            write(connfd, snd, strlen(snd) + 1);
        }
    }

    pthread_exit(NULL);
}


int CheckDis(char *str)
{
    char *pos;
    if ((pos = strstr(str, "Baseball Stadium")) != NULL) return 0;
    else if ((pos = strstr(str, "Big City")) != NULL) return 1;
    else if ((pos = strstr(str, "Neiwan Old Street")) != NULL) return 2;
    else if ((pos = strstr(str, "NYCU")) != NULL) return 3;
    else if ((pos = strstr(str, "Smangus")) != NULL) return 4;
    else if ((pos = strstr(str, "17 km of Splendid Coastline")) != NULL) return 5;
    else if ((pos = strstr(str, "100 Tastes")) != NULL) return 6;
    else if ((pos = strstr(str, "Science Park")) != NULL) return 7;
    else if ((pos = strstr(str, "City God Temple")) != NULL) return 8;
    else return -1;
}


void refresh_data(refresh_unit unit)
{
    P(s, 10);
    for (int i = 0; i < 9; i++){
        if(unit.Type[i] == 1)
        {
            dist_data[i][0] += unit.data[i][0];
            dist_data[i][1] += unit.data[i][1];
            dist_data[i][2] += unit.data[i][2];
            dist_data[i][3] += unit.data[i][3];
        }
    }
    V(s, 10);
}

int ifrepet(refresh_unit unit)
{
    for (int i = 0; i < 9; i++){
        if ((Onwait[i] == 1) && (unit.Type[i] == 1)) 
            return i;
    }
    return -1;
}

void refresh_bustime(int wait, int bus_index)
{
    if(bus_index == 0){
        P(s, 12);
        bus[0].time += wait;
        V(s, 12);
    }
    else if(bus_index == 1){
        P(s, 13);
        bus[1].time += wait;
        V(s, 13);
    }

}

void socket_connetion(char* port)
{
    struct sockaddr_in ser_addr, client_addr;

	// create a socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0){
		printf("Fail to create a socket.");
		exit(0);
	}

	int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	// set socket attribute
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = INADDR_ANY;
    ser_addr.sin_port = htons((unsigned short)atoi(port));

	// socket binding
	if (bind(server_fd, (const struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0) {
		printf("Bind socket failed!");
		close(server_fd);
		exit(0);
	}
    // socket listening
    listen(server_fd, 50);
}