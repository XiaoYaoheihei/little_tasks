/*
创建一个新的进程，然后在新的进程中执行对应的功能
问题：
1.按下ctl-d和exit之后不能完整的退出程序，会一直循环写提示符
2.还不能够在一行中输入所有参数（当前所有的版本都是先将参数放进一个缓冲区中，然后从缓冲区中读取字符串运行）
3.按下ctl-c之后连同运行shell的程序也退出了
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAXARGS 20      //命令行参数
#define ARGLEN 100      //参数长度

int execute(char **);
int main() {
    char *arglists[MAXARGS + 1];
    int numargs;        //数组索引
    char argbuf[ARGLEN];//读取命令行缓冲区
    char *makestring();
    numargs = 0;
    while (numargs < MAXARGS) {
        if (numargs == 0) {
            printf("##\n");
        }
        printf("Arg[%d]? ",numargs);
        if (fgets(argbuf, ARGLEN, stdin) && *argbuf != '\n') {//没有读取到换行符的时候一直读
            arglists[numargs++] = makestring(argbuf);
        } else {
            if (numargs > 0) {
                arglists[numargs] = NULL;
                execute(arglists);
                numargs = 0;
            }
        }
    }
    return 0;
}

int execute(char *arglist[]) {
    //使用fork和execvp和wait函数处理
    int pid, exitstatus;
    pid = fork();
    switch (pid)
    {
    case -1:
        perror("fork faild");
        exit(1);
    case 0:
        execvp(arglist[0], arglist);
        perror("execvp faild");
        exit(1);
    default:
        while (wait(&exitstatus) != pid)    
        ;                                   //如果返回的不是进程pid，理应有对应的处理函数
        printf("child exited with status %d, %d\n",
        exitstatus >> 8, exitstatus & 0377);
        break;
    }
}

char *makestring(char *buf) {
    //删除换行符并为字符串创建存储空间
    char *cp;
    buf[strlen(buf) - 1] = '\0';    //修改换行符，改为字符串末尾
    cp = (char *)malloc(strlen(buf) + 1);   //动态申请内存空间
    if (cp == NULL) {               //申请是否成功
        fprintf(stderr, "no memory\n");
        exit(1);
    }
    strcpy(cp, buf);                //重新赋值
    return cp;                      //返回申请成功的堆内存地址
}

