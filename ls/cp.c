#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

//此代码只能在当前目录的情况下实现cp命令
//考虑以下是否可以转换目录实现

# define BUFFSIZE 4096
void oops(char *, char *);

int main(int ac, char *av[]) {
    int start_fd, end_fd;
    ssize_t current;    //当前实际写入的字节数量
    char buf[BUFFSIZE];//设置的缓冲区域
    if (ac != 3) {
        //printf("%d", ac);
        fprintf(stderr, "error");
        exit(1);
    }
    //开始读取文件信息并且将他们写入指定的文件当中去
    if (start_fd = open(av[1], O_RDONLY) == -1) {
        oops("Cannot open", av[1]);
    } 
    if (end_fd = open(av[2], O_RDONLY) == -1) {
        oops("Cannot open", av[2]);
    }
    while(current = read(start_fd, buf, BUFFSIZE) > 0) {
            if(write(end_fd, buf, BUFFSIZE) < current) {
                oops("write error to ", av[2]);
            }
    }
    
    if (close(start_fd) == -1 || close(end_fd) == -1) {
        printf("Cannot close the file");
    }
    return 0;
}

void oops(char *s1, char *s2) {
    fprintf(stderr, "error : %s\n", s1);
    perror(s2);
    exit(1);
}
//目前可以打开文件，但是不能写入，搞不太懂什么意思