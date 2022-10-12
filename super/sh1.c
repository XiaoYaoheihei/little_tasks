//带有提示符的shell
/*
程序运行正常，一如既往地被execvp覆盖了shell的程序代码，然后在命令指定的程序结束后退出。
shell不会在接受新的命令，为了运行新的命令，只能再次运行shell。
那有没有什么解决方案吗？
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

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
    //使用execvp函数处理
    execvp(arglist[0], arglist);    
    perror("execvp failed");
    exit(1);
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