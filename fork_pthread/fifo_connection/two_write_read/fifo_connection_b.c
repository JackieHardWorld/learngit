#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <strings.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define MY_FIFO     "bfifo"//和fifo_connection_a相比,b只是调换了一下管道名（读端变写端，写端变读端）
#define DEST_FIFO   "afifo"

int main(void)
{
    if(access(MY_FIFO, F_OK) != 0)
    {
        mkfifo(MY_FIFO, 0664);
    }

    if(access(DEST_FIFO, F_OK) != 0)
    {
        mkfifo(DEST_FIFO, 0664);
    }

    int myfifo_fd;
    int destfifo_fd;

    myfifo_fd = open(MY_FIFO, O_RDWR);
    destfifo_fd = open(DEST_FIFO, O_RDWR);

    pid_t pid;
    char buffer[100]={0};

    // pid = fork();
    // if(pid == 0)
    // {
        

    //     while(1)
    //     {
    //         memset(buffer, 0, sizeof(buffer));
    //         read(myfifo_fd, buffer, sizeof(buffer));
    //         printf("有个xx给我发消息：%s\n", buffer);
    //     }

    //     exit(0);
    // }

    // while(1)
    // {
    //     scanf("%s", buffer);

    //     write(destfifo_fd, buffer, strlen(buffer));
    // }

    fcntl(myfifo_fd, F_SETFL, O_NONBLOCK);
    fcntl(0, F_SETFL, O_NONBLOCK);
    int retval;
    ssize_t rd_size;

    while(1)
    {
        retval = scanf("%s", buffer);
        if(retval == 1)
            write(destfifo_fd, buffer, strlen(buffer));

        rd_size = read(myfifo_fd, buffer, sizeof(buffer));
        if(rd_size > 0)
        {
            printf("有个xx给我发消息：%s\n", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
    }



    wait(NULL);

    close(destfifo_fd);
    close(myfifo_fd);

    return 0;
}