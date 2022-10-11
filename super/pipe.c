//他使用两个程序的名字作为参数运行程序
//用法：pipe command1 command2
//等效效果：command1 | command2
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#define oops(m, x) {perror(m); exit(x);}

int main(int ar, char **av) {
    int thepipe[2];
    int newfd;
    int pid;
    
    if (ar != 3) {
        fprintf(stderr, "error:pipe cmd1 cmd2\n");
        exit(1);
    }
    //创建管道
    if (pipe(thepipe) == -1) {              
        oops("cannot get a pipe", 1);
    }
    //创建管道成功之后再进行fork
    if ((pid = fork()) == -1) {
        oops("cannot fork", 2);
    }
    //父进程会从管道里面读取数据
    if (pid > 0) {
        close(2);
        close(thepipe[1]);  //父进程关闭写管道
        if (dup2(thepipe[0], 0) == -1) {
            oops("could not redirect stdin", 3);
        }
    
        close(thepipe[0]);  //标准输入已经改变成为管道的读取端
        execlp(av[2], av[2], NULL);
        oops(av[2], 4);
    }
    //子进程向管道里面写数据
    if (pid == 0) {
        close(thepipe[0]);  //关闭读取管道
        if (dup2(thepipe[1], 1) == -1) {
            oops("could not redirect stdout", 4);
        }
        close(thepipe[1]);  //标准输出已经改变为管道的写入端
        execlp(av[1], av[1], NULL);
        oops(av[1], 5);
    }
    return 0;
}