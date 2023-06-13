#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int fd;
    int len = strlen(argv[1]);
    char* latter = (char*)malloc((len+1)* sizeof(char));
    strcpy(latter, argv[1]);
    fd = open("/dev/mydev", O_WRONLY);

    if (fd < 0) {
        printf("Error opening device file!\n");
        return 1;
    }
    
    char output;
    for(int i = 0; i < (len+1); i++){
        output = latter[i];
        // if((fd = write(fd, &output, 1)) == -1) {
        // printf("Error write to server\n");
        // exit(-1);
        // }
        write(fd, &output, 1);
        sleep(1);
        printf("%c\n", output);
    }

    close(fd);

    return 0;
}