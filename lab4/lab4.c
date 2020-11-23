#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void printdir(char *dir, int depth);
void printinfo(struct stat *statbuf, int depth, char *name);

int count = 0; //记录文件总量

int main(int argc, char *argv[])
{
    char path[256];
    if(argc == 1)
    {
        printf("Please enter the DIR path!\n");
        return 1;
    }
    strcpy(path, argv[1]);
    printdir(path, 0);
    printf("Num of files is %d\n", count);
    return 0;
}

void printdir(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry; //目录项信息结构体
    struct stat statbuf;  //保存文件信息的结构体
    if ((dp = opendir(dir)) == NULL)
    {
        perror("Opendir failed: ");
        return;
    }
    if (chdir(dir) == -1)
    {
        perror("Chdir dir failed: ");
        return;
    }
    while ((entry = readdir(dp)) != NULL) //读到一个目录项
    {
        count++;
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue; //目录项名字为"."或者".."，跳过该目录项
            printinfo(&statbuf, depth, entry->d_name);
            printdir(entry->d_name, depth + 4);
        }
        else
            printinfo(&statbuf, depth, entry->d_name);
    }
    if (chdir("..") == -1)
    {
        perror("Chdir .. failed: ");
        return;
    }
    closedir(dp);
}

void printinfo(struct stat *statbuf, int depth, char *name)
{
    struct passwd *pd;
    struct group *gp;
    struct tm t;
    localtime_r(&(statbuf->st_mtime), &t);
    char timebuf[30];
    strftime(timebuf, 30, "%Y-%m-%d %H:%M", &t);    //格式化最后修改时间

    //打印深度
    printf("Depth:%2d  ", depth);

    //打印文件类型和文件权限
    if (S_ISDIR(statbuf->st_mode))
        printf("d"); //目录
    else if (S_ISLNK(statbuf->st_mode))
        printf("l"); //链接文件
    else
        printf("-"); //普通文件
    //创建者权限
    if ((S_IRUSR & statbuf->st_mode) == S_IRUSR)    //读取权限
        printf("r");
    else
        printf("-");
    if ((S_IWUSR & statbuf->st_mode) == S_IWUSR)    //写入权限
        printf("w");
    else
        printf("-");
    if ((S_IXUSR & statbuf->st_mode) == S_IXUSR)    //执行权限
        printf("x");
    else
        printf("-");
    //创建者所在组其他用户权限
    if ((S_IRGRP & statbuf->st_mode) == S_IRGRP)
        printf("r");
    else
        printf("-");
    if ((S_IWGRP & statbuf->st_mode) == S_IWGRP)
        printf("w");
    else
        printf("-");
    if ((S_IXGRP & statbuf->st_mode) == S_IXGRP)
        printf("x");
    else
        printf("-");
    //其它组用户权限
    if ((S_IROTH & statbuf->st_mode) == S_IROTH)
        printf("r");
    else
        printf("-");
    if ((S_IWOTH & statbuf->st_mode) == S_IWOTH)
        printf("w");
    else
        printf("-");
    if ((S_IXOTH & statbuf->st_mode) == S_IXOTH)
        printf("x");
    else
        printf("-");
    printf(" ");
    printf("%2lu ", statbuf->st_nlink); //一级子目录或链接个数
    if ((pd = getpwuid(statbuf->st_uid)) == NULL)
        perror("Getpwuid failed: ");
    else
        printf(" %s ", pd->pw_name); //创建者名
    if ((gp = getgrgid(statbuf->st_gid)) == NULL)
        perror("Getgrgid failed: ");
    else 
        printf("%s ", gp->gr_name); //所在组名
    printf("%5lu\t", statbuf->st_size);
    printf("%s ", timebuf);
    printf("%s\n", name);
}