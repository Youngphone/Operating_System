#include "lab3.h"

int semid = 0;      //信号灯集id
int shmid[2 * BUFNUM];  //共享存储区数组
char *addr[BUFNUM]; //指针数组
int fp = 0;
int *writenum[BUFNUM];

void put()
{
    fp = open("output", O_WRONLY | O_CREAT | O_TRUNC);
    int i = 0;
    int putnum = 0;
    while (1)
    {
        P(semid, 0); //对满的共享存储区进行P操作
        P(semid, 2); //共享存储区互斥锁上锁
        putnum = *writenum[i];
        write(fp, addr[i], putnum);
        memset(addr[i], 0, putnum);
        V(semid, 2); //开锁
        printf("Process put: 从共享存储区获取%d字节数据.\n", putnum);
        i = (i + 1) % BUFNUM;
        V(semid, 1);           //对空的共享存储区进行V操作
        if (putnum != BUFSIZE) //输入已到文件尾
            break;
    }
}

int main()
{
    printf("Process put: start!\n");
    semid = semget((key_t)SEMKEY, 3, IPC_CREAT | 0666);
    printf("Process put: 获取信号量成功!\n");
    for (int i = 0; i < BUFNUM; i++)
    {
        //创建共享存储区
        shmid[i] = shmget((key_t)(SHMKEY + i), BUFSIZE, 0666 | IPC_CREAT);
        //附接共享存储区
        addr[i] = shmat(shmid[i], NULL, 0);
    }
    for (int i = 0; i < BUFNUM; i++)
    {
        shmid[BUFNUM + i] = shmget((key_t)(SHMKEY + BUFNUM + i), sizeof(int), 0666 | IPC_CREAT);
        writenum[i] = shmat(shmid[BUFNUM + i], NULL, 0);
    }
    printf("Process put: 获取共享存储区成功!\n");
    put();
    for (int i = 0; i < BUFNUM; i++)
        shmdt(addr[i]);
    printf("Process put: end!\n");
    return 0;
}