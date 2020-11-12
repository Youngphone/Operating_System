#include <stdio.h>
#include <stdlib.h>
#include <linux/sem.h>
#include <pthread.h>
#include <sys/types.h>

#define NUMSIZE 100

//P操作接口
void P(int semid, int index);
//V操作接口
void V(int semid, int index);
//计算线程
void *compute();
//打印线程1
void *print_1();
//打印线程2
void *print_2();

int a = 0;            //公共变量
int semid = 0;        //信号灯集id
pthread_t p1, p2, p3; //线程id
union semun full, empty;
int count = 0; //判断计算是否完成

int main()
{
    semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666); //创建信号灯
    full.val = 0;
    empty.val = 1;
    semctl(semid, 0, SETVAL, full);
    semctl(semid, 1, SETVAL, empty);
    pthread_create(&p1, NULL, compute, NULL);
    pthread_create(&p2, NULL, print_1, NULL);
    pthread_create(&p3, NULL, print_2, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    semctl(semid, 0, IPC_RMID);
    return 0;
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

void *compute()
{
    int i = 0;
    for (i = 1; i <= NUMSIZE; i++)
    {
        P(semid, 1);
        a = a + i;
        V(semid, 0);
    }
}

void *print_1()
{
    while (1)
    {
        P(semid, 0);
        if (count == NUMSIZE)
        {
            V(semid, 0);
            break;
        }
        if (a % 2 == 0)
        {
            printf("Print pthread 1 print: %d\n", a);
            count++;
            if (count < NUMSIZE)
                V(semid, 1);
            else
                V(semid, 0);
        }
        else
            V(semid, 0);
    }
}

void *print_2()
{
    while (1)
    {
        P(semid, 0);
        if (count == NUMSIZE)
        {
            V(semid, 0);
            break;
        }
        if (a % 2 != 0)
        {
            printf("Print pthread 2 print: %d\n", a);
            count++;
            if (count < NUMSIZE)
                V(semid, 1);
            else
                V(semid, 0);
        }
        else
            V(semid, 0);
    }
}