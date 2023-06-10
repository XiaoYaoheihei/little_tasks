#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<utmp.h>
#include<fcntl.h>
#include<time.h>
//who命令的简单实现
//用户名，终端名，登陆时间，远程计算机名称

void show_inf(struct utmp *);
void check_time(long );

int main() {
    struct utmp current;    //从这个文件的结构体中读取用户的相关信息
    int currentfd;          //文件描述符
    if (currentfd = (open(UTMP_FILE, O_RDONLY)) == -1) {//以只读的方式打开这个文件
        perror(UTMP_FILE);
        exit(1);            //异常退出
    }
    while (read(currentfd, &current, sizeof(current)) == sizeof(current)) {
        show_inf(&current); //显示这个结构中的信息
    }
    close(currentfd);
    return 0;
}

void show_inf(struct utmp *currentfd) {
    if (currentfd->ut_type != USER_PROCESS) {//首先判断用户名是不是已经注册过的
        return ;
    }
    printf("%-8.8s", currentfd->ut_name);   //用户名
    printf(" ");
    printf("%-8.8s", currentfd->ut_line);   //终端名称
    printf(" ");
    check_time(currentfd->ut_time);         //登陆时间
    //printf("%12.12s", 4 + ctime(&currentfd->ut_time));
    if (currentfd->ut_host[0] != '/0') {
        printf("(%s)", currentfd->ut_host); //远程计算机名称
    }
    printf("\n");
}

void check_time(long time) {
    char *pos;                      //保存时间的地址
    pos = ctime(&time);
    printf("%12.12s", pos + 4);      //从第四个位置输出
}
//ctime函数要输入一个指向time_t的指针哟
//和1.c好好对比一下，有一些小问题需要着重看看

//这个版本的who已经完整的实现了who这个命令，但是还有一个问题，
//就是缓冲区：读一次写一次非常浪费时间，可否读多次放入缓冲区里面再一次性写入
//缓冲区的大小问题是一个需要好好考虑的知识点