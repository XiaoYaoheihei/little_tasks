#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdlib.h>
#include<unistd.h>

//对于递归手动实现                               
//和用本身自带的接口来实现
void recursion(char *);

int main(int argc, char **argv) {
    if (argc == 1) {
        //printf("only one demo\n\n");
        recursion(".");           //传进去当前目录的地址
    } else {        //访问你指定的目录的地址
            //printf("%s:\n ", * ++argv);
        recursion(*++argv);
    }
    return 0;
}

void recursion( char *path) {
    char name[100];                 
    int count = 0;                  //文件数目
    DIR *dir;
    struct dirent *ptr;
    if (chdir(path) < 0) {          //首先切换到当前目录
        perror("chdir");
    }
    if (getcwd(name, 100) < 0) {    //获取当前绝对路径
        perror("getcwd");
    }
    dir = opendir(name);            //打开当前目录
    if (dir == NULL) {
        perror("opendir");
    }
    while ((ptr = readdir(dir)) != NULL) {//计数
       count++;
    }
    close(dir);
    //存储文件名字的动态数组
    char **filenames = (char **)malloc(count*sizeof(char *));
    memset(filenames, 0, sizeof(char*)*count);

    for(int i = 0; i < count; i++) {
        filenames[i] = (char *)malloc(256*sizeof(char));
        memset(filenames[i], 0 , sizeof(char)*256);
    }

    dir = opendir(name);
    for(int i = 0; i < count; i++) {
        ptr = readdir(dir);                 //读取目录流里的文件内容
        if (ptr == NULL) {
            perror(readdir);
        }
        strcat(filenames[i], ptr->d_name);  //文件名赋值
    }

    //打印文件名字即可
    for (int i = 0; i < count ;i++) {
        printf("%s", filenames[i]);
    }

        struct stat buf;                        //文件属性信息
        for (int i = 0; i < count; i++) {
            if (stat(filenames[i], &buf) == -1) {
                perror("stat");
            }
            if (strcmp(filenames[i], ".") == 0) {//当前目录
                continue;
            } else if (strcmp(filenames[i], "..") == 0) {//父目录
                continue;
            }
            if (S_ISDIR(buf.st_mode)) {             //如果是目录进入递归
                recursion(filenames[i]);
            } 
        }
    }
    

