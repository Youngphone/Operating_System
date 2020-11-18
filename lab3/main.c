#include "lab3.h"

int semid = 0;         //信号灯集id
int shmid[2 * BUFNUM]; //共享存储区数组

int main()
{
    semid = semget((key_t)SEMKEY, 3, IPC_CREAT | 0666);
    printf("Process main: 创建信号灯成功!\n");
    semctl(semid, 0, SETVAL, 0);  //满
    semctl(semid, 1, SETVAL, 10); //空
    semctl(semid, 2, SETVAL, 1);  //互斥
    for (int i = 0; i < BUFNUM; i++)
    {
        shmid[i] = shmget((key_t)(SHMKEY + i), BUFSIZE, 0666 | IPC_CREAT);
    }
    //用于存储每次读取文件内容的长度
    for(int i = 0; i < BUFNUM; i++)
        shmid[BUFNUM + i] = shmget((key_t)(SHMKEY + BUFNUM + i), sizeof(int), 0666 | IPC_CREAT);
    printf("Process main: 创建共享存储区成功!\n");
    pid_t pid1, pid2;
    pid1 = fork();
    if (pid1 == -1)
    {
        perror("pid1 fork error");
        exit(-1);
    }
    if (pid1 == 0)
    {
        printf("Process get 创建成功!\n");
        execv("./get", NULL);
    }
    else
    {
        pid2 = fork();
        if (pid2 == -1)
        {
            perror("pid2 fork error");
            exit(-1);
        }
        if (pid2 == 0)
        {
            printf("Process put 创建成功!\n");
            execv("./put", NULL);
        }
        else
        {
            wait(NULL);
            wait(NULL);
            semctl(semid, 0, IPC_RMID);
            printf("Process main: 信号量删除成功.\n");
            for (int i = 0; i < 2 * BUFNUM; i++)
            {
                shmctl(shmid[i], IPC_RMID, 0);
            }
            printf("Process main: 共享存储区删除成功.\n");
        }
    }
    return 0;
}