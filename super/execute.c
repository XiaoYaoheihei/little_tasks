#include "smsh.h"
/*
运行一个传递参数的程序
*/
int execute(char *argv[]) {
    int pid;
    int child_info = -1;
    if (argv[0] == NULL) {
        return 0;
    }
    if ((pid = fork()) == -1) {
        perror("fork");
    } else if (pid == 0) {
        //子进程对应的操作
        //屏蔽掉ctl-c，不会屏蔽ctl-d，在执行execvp之前
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        execvp(argv[0], argv);
        perror("cannot execute command");
        exit(1);
    } else {
        if (wait(&child_info) == -1) {//父进程等待子进程退出
            perror("wait");
        }
    }
    return child_info;
}