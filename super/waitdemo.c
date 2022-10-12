/*
不仅显示了子进程的退出，而且还显示了子进程的退出状态
wait函数的返回值：
-1      遇到错误
pid     结束进程的pid
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define DELAY 5
void child_code(int );
void parent_code(int );

int main () {
    int newid;
    printf("before: mypid is %d\n", getpid());
    if ((newid = fork()) == -1) {
        perror("fork");
    } else if (newid == 0) {
        child_code(DELAY);
    } else {
        parent_code(newid);
    }
    return 0;
}

void child_code(int delay) {
    //子进程休息之后退出
    printf("child %d here will sleep for %d seconds\n", getpid(), delay);
    sleep(delay);
    printf("child done,about to exit\n");
    exit(17);
}

void parent_code(int childpid) {
    //父进程等待子进程退出之后打印相关信息
    int wait_rv;        //wait函数的返回值
    int child_status;
    int high_8, low_7, bit_7;

    wait_rv = wait(&child_status);
    printf("done waiting for %d. waiting returned: %d \n", childpid, wait_rv);

    high_8 = child_status >> 8;             //1111 1111 0000 0000
    low_7 = child_status & 0x7F;            //0000 0000 0111 1111
    bit_7 = child_status & 0x80;            //0000 0000 1000 0000
    printf("status : exit = %d, sig = %d, core = %d\n", high_8, low_7, bit_7);    
}