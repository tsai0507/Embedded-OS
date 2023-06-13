#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h> // waitpid()

pid_t childpid;
int server_fd;

void handler(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}
void childfunc(int new_socket)
{
    // const char *welcome = "Welcome to the server!";
    // send(new_socket, welcome, strlen(welcome), 0);
    dup2(new_socket, STDOUT_FILENO);
    execl("/usr/games/sl", "/usr/games/sl", "-l", NULL);
    execlp("sl", "sl","-l", NULL);
    close(new_socket);
	exit(0);
}

int main(int argc, char const *argv[])
{
    
    struct sockaddr_in address;
    int opt = 1, yes = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    // signal(SIGINT, handler1);
    signal(SIGCHLD, handler);

    // 建立socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 設定socket選項，防止地址被占用
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }


    // 設定socket地址
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((u_short)atoi(argv[1]));

    // 綁定socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    

    // 監聽socket，最多允許5個client連接
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("等待client連接...\n");

    int new_socket;
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen))) {
        // 與 client 進行通訊

        childpid = fork();
        if (childpid >= 0) { /* fork succeeded */
            if (childpid == 0) { /* fork() returns 0 to the child process */
                childfunc(new_socket);
            } else { /* fork() returns new pid to the parent process */
                printf("Train ID: %d\n",childpid);
            }
        } else { /* fork returns -1 on failure */
            perror("fork error"); /* display error message */
            exit(0);
        }
    }
    close(server_fd);


    return 0;
}
