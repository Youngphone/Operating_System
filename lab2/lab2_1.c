#include <stdio.h>
#include <stdlib.h>
#include <linux/sem.h>
#include <pthread.h>
#include <sys/types.h>

#define PTHREADSIZE 5  //线程数量
#define TICKETSIZE 1000 //机票总数

//P操作接口
void P(int semid, int index);
//V操作接口
void V(int semid, int index);
//售票线程
void *sell();

int soldcount = 0;        //已售机票数量
int semid = 0;            //信号灯集id
pthread_t p[PTHREADSIZE]; //售票线程id
union semun arg;

int main()
{
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666); //创建信号灯
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);
    int a = 0;
    for (a = 0; a < PTHREADSIZE; a++)
    {
        pthread_create(&p[a], NULL, sell, NULL);
    }
    for (a = 0; a < PTHREADSIZE; a++)
    {
        pthread_join(p[a], NULL);
    }
    printf("All tickets have sold out.\n");
    semctl(semid, 0, IPC_RMID);
    return 0;
}

void *sell()
{
    int count = 0;
    while (1)
    {
        P(semid, 0);
        if (soldcount < TICKETSIZE)
        {
            soldcount++;
            count++;
            //printf("Pthread %u sold a ticket.\n", pthread_self());
            V(semid, 0);
        }
        else
        {
            V(semid, 0);
            break;
        }
    }
    printf("Pthread %u have sold %d tickets.\n", pthread_self(), count);
}

void P(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

void V(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}