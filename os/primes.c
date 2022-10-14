#include "kernel/types.h"
#include "user/user.h"

void child(int *f_pipe) {
    close(f_pipe[1]);       //关闭写端
    int cur;            //记录第一次得到的数字
    if (read(f_pipe[0], &cur, sizeof(int)) != sizeof(int)) {   
        //如果读到的数字不是int类型的话，就关闭管道并退出进程
        close(f_pipe[0]);
        exit(0);
    }
    printf("prime %d\n", cur);

    int num = 0;        //记录第二次之后得到的数字
    int thepipe[2];
    int re = pipe(thepipe);
    if (re == -1) {
        exit(-1);
    }

    if (fork() == 0) {
        child(thepipe);     //进入递归
    } else {
        close(thepipe[0]);            //关闭读端
        while (read(f_pipe[0], &num, sizeof(int)) == sizeof(int)) {
            if (num % cur == 0) {     //是倍数的话就舍弃掉
                continue;
            } 
            write(thepipe[1], &num, sizeof(int));    //不是倍数的话写入管道
        }
        close(thepipe[1]);          //数字写完了就关闭写端
        wait(0);
    }
    
    //千万记得要用exit函数，要不然就成僵尸进程了（父进程没有办法得到返回值）
    exit(0);
}

int main(int argc, char **argv) {
    int thepipe[2];
    int re = pipe(thepipe);
    if (re == -1) {
        exit(-1);
    }
    if (fork() > 0) {
        //父进程
        close(thepipe[0]);          //关闭读端
        for(int i = 2; i <= 35; i++) {
            write(thepipe[1], &i, sizeof(int)); //向写端写入数据
        }
        close(thepipe[1]);          //必须关闭管道，否则子进程在read函数阻塞
        wait(0);                    //等待子进程退出
    } else {
        //子进程
        child(thepipe);         //将第一个管道参数传输进去
    }
    
    exit(0);
}