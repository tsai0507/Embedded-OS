#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

typedef struct {
    int guess;
    char result[8];
}data;
sig_atomic_t flag = 0;

void handler (int signal_number)
{
    flag = 1;
}

int main(int argc, char *argv[])
{
    int shmid, guess = 0, bound = 0, result;
    pid_t pid_game;
    const char* bigger = "bigger";
    const char* smaller = "smaller";
    const char* bingo = "bingo";
    key_t key;
    data *sh_data, *shm;
    struct sigaction my_action;
    struct timespec req, rem;

    if (argc != 4){
        fprintf(stderr,"%s: specify a key (long), guess value (int), PID of game\n",argv[0]);
        exit(1);
    }
    key = atoi(argv[1]);
    bound = atoi(argv[2]);
    guess = bound/2;
    pid_game = atoi(argv[3]);
    

    /* set the sleep time to 1 sec */
    memset(&req, 0, sizeof(struct timespec));
    req.tv_sec = 1;
    req.tv_nsec = 0;

    /* register handler to SIGUSR1 */
    memset(&my_action, 0, sizeof (struct sigaction));
    my_action.sa_handler = handler;
    sigaction (SIGUSR1, &my_action, NULL);

    /* Create the share memory */
    shmid = shmget(key, sizeof(data), IPC_CREAT | 0666);
    /* Now we attach the segment to our data space */
    if ((shm = shmat(shmid, NULL, 0)) == (data *) -1) {
        perror("shmat");
        exit(1);
    }
    sh_data = shm;


    while(1){
        result = nanosleep(&req, &rem); 
        if (result == -1)
            nanosleep(&rem, &rem);

        if(flag == 1){
            if (strstr(sh_data->result, bigger) != NULL)
            {
                guess = (bound - guess)/2 + guess;
            }
            else if (strstr(sh_data->result, smaller) != NULL)
            {
                bound = guess;
                guess /= 2;
            }
            else if (strstr(sh_data->result, bingo) != NULL)
                break;
            flag = 0;
        }
        printf("[Game] Guess: %d\n", guess);
        sh_data->guess = guess;
        kill(pid_game, SIGUSR1); 
    } 

    /* Detach the share memory segment */
    shmdt(shm);

    return 0;
}