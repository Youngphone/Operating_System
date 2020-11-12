#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MSGSIZE 20

void handlefunc(int signum);

pid_t pid1, pid2;
int pipefd[2];
int count = 0;

int main()
{
    char buf[50];
    //创建无名管道
    if(pipe(pipefd) == -1)
    {
        perror("pip error");
        exit(-1);
    }
    pid1 = fork();
    if(pid1 == -1)
    {
        perror("pid1 fork error");
        exit(-1);
    }
    if(pid1 == 0)
    {
        signal(SIGINT, SIG_IGN);  //忽略信号SIGINT
        signal(SIGUSR1, handlefunc);
        close(pipefd[1]);
        while(1)
        {
            if(read(pipefd[0], buf, sizeof(buf)) == 0)
                break;
            printf("Child Process 1 receive: %s", buf);
            count++;
        }
        close(pipefd[0]);
        printf("Child Process 1 has received %d message.\n", count);
    }
    else
    {
        pid2 = fork();
        if(pid2 == -1)
        {
            perror("pid2 fork error");
            exit(-1);
        }
        if(pid2 == 0)
        {   
            signal(SIGINT, SIG_IGN);  //忽略信号SIGINT
            signal(SIGUSR2, handlefunc);
            close(pipefd[1]);
            while(1)
            {
                if(read(pipefd[0], buf, sizeof(buf)) == 0)
                    break;
                printf("Child Process 2 receive: %s", buf);
                count++;
            }
            close(pipefd[0]);
            printf("Child Process 2 has received %d message.\n", count);
        }
        else
        {
            signal(SIGINT, handlefunc);
            close(pipefd[0]);
            while(count < MSGSIZE)
            {
                count++;
                sprintf(buf, "I send you %d times.\n", count);
                write(pipefd[1], buf, sizeof(buf));
                sleep(1);
            }
            close(pipefd[1]);
            wait(&pid1);
            wait(&pid2);
            printf("All message have been send!\n");
            printf("Exit normally!\n");
        }
    }
    return 1;
}

//信号处理函数
void handlefunc(int signum)
{
    if(signum == SIGUSR1)
    {
        printf("Child Process 1 is Killed by Parent!\n");
        printf("Child Process 1 has received %d message.\n", count);
        exit(0);
    }
    if(signum == SIGUSR2)
    {
        printf("Child Process 2 is Killed by Parent!\n");
        printf("Child Process 2 has received %d message.\n", count);
        exit(0);
    }
    if(signum == SIGINT)
    {
        kill(pid1, SIGUSR1);
        kill(pid2, SIGUSR2);
        wait(&pid1);
        wait(&pid2);
        close(pipefd[0]);
        close(pipefd[1]);
        printf("Parent Process is Killed!\n");
        printf("Abnormal Exit!\n");
        exit(0);
    }
}