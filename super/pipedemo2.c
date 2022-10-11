//将pipe和fork结合起来，创建一对管道来通信的进程

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

#define BUFSIZE 5
#define CHILD "I want a cookie\n"
#define PARENT "testing..\n"
//管道写的过程我们是看不到的，我们只能看到的是进程中的文件描述符
int main () {
    int pipefd[2];
    int len;
    char buf[BUFSIZE];
    int read_len;
    if (pipe(pipefd) == -1) {
        exit(1);
    }
    switch (fork())
    {
    case -1/* constant-expression */: {
            exit(2);
        }
        //child
    case 0:{
        close(pipefd[0]);
        len = strlen(CHILD);
        while (1) {
            if (write(pipefd[1], CHILD, len) != len) {
                exit(3);
            }
            sleep(5);
        }
    }
    //parent
    default:{
        len = strlen(PARENT);
        while (1) {
            // if (write(pipefd[1], PARENT, len) != len) {
            //     exit(4);
            // }
            close(pipefd[1]);
            sleep(1);
            read_len = read(pipefd[0], buf, BUFSIZE);
            if (read_len <= 0) {
                break;
            }
            write(1, buf, read_len);
        }
    }
        
    }
 }