#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

typedef struct {
    int guess;
    char result[8];
}data;

sig_atomic_t flag = 0;
pid_t pid_guess;
int shmid = 0;

void guess_handler (int signo, siginfo_t *info, void *context)
{
    pid_guess = info->si_pid;
    flag = 1;
}

void sigintHandler(int sig_num)
{
    /* Destroy the share memory segment */
    printf("Server destroy the share memory.\n");
    if (shmctl(shmid, IPC_RMID, NULL) < 0)
    {
        fprintf(stderr, "Server remove share memory failed\n");
        exit(1);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int guess_ans = 0, guess = 0;
    const char* bigger = "bigger";
    const char* smaller = "smaller";
    const char* bingo = "bingo";
    key_t key;
    data *sh_data, *shm;
    struct sigaction my_action;


    if (argc != 3){
        fprintf(stderr,"%s: specify a key (long) and guess value (int)\n",argv[0]);
        exit(1);
    }
    key = atoi(argv[1]);
    guess_ans = atoi(argv[2]);
    printf("[Game] PID: %d\n", getpid());

    memset(&my_action, 0, sizeof (struct sigaction));
    my_action.sa_flags = SA_SIGINFO;
    my_action.sa_sigaction = &guess_handler;
    sigaction(SIGUSR1, &my_action, NULL);
    signal(SIGINT, sigintHandler);

    /* Create the share memory */
    shmid = shmget(key, sizeof(data), IPC_CREAT | 0666);
    

    /* Now we attach the segment to our data space */
    if ((shm = shmat(shmid, NULL, 0)) == (data *) -1) {
        perror("shmat");
        exit(1);
    }
    printf("Server create and attach the share memory.\n");
    sh_data = shm;
    sh_data->guess = 0;

    while(1){
        if(flag == 1){
            flag = 0;
            guess = sh_data->guess;
            memset(sh_data->result, 0, sizeof(sh_data->result));
            if(guess == guess_ans)
                strcpy(sh_data->result, bingo);
            else if(guess > guess_ans) 
                strcpy(sh_data->result, smaller);  
            else if(guess < guess_ans)
                strcpy(sh_data->result, bigger);
            printf("[Game] Guess %d, %s\n", guess, sh_data->result);
            kill(pid_guess, SIGUSR1);    
        }
    }

    return 0;
}