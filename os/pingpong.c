#include "kernel/types.h"
#include "user/user.h"
//目前程序碰到了一点小问题，可以打印出来最终想要的结果，但是终端显示：
//usetrap()问题（有内存页表错误）
int main(int argc, char **argv) {
    //两个管道
    int pipe1[2];
    int pipe2[2];
    pipe(pipe1);
    pipe(pipe2);
    int pid = fork();

    if (pid == -1) {
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        exit(1);
    } else if (pid > 0) {
        //父进程
        char now;
        write(pipe1[1], &now, sizeof(now));
        // sleep(1);
        int re = read(pipe2[0], &now, sizeof(now));
        if (re == -1) {
            exit(1);
        }
        printf("%d: receving pong", getpid());
    } else {
        //子进程
        char a;
        int result = read(pipe1[0], &a, sizeof(a));
        if (result == -1) {
            exit(1);
        }
        printf("%d: receving ping\n", getpid());
        write(pipe2[1], &a, sizeof(a));
    }
    return 0;
}