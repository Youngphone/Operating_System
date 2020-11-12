#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <wait.h>

#define BUFSIZE 1024 //缓冲区大小
#define BUFNUM 10    //缓冲区数量
#define SEMKEY 1111  //信号灯键值
#define SHMKEY 6666  //共享存储区键值

//P操作接口
void P(int semid, int index);
//V操作接口
void V(int semid, int index);

int semid = 0; //信号灯集id

int main()
{
    semid = semget((key_t)SEMKEY, 2, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 0);
    semctl(semid, 1, SETVAL, 1);
    
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