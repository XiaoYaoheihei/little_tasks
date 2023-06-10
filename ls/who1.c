#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<utmp.h>
#include<fcntl.h>
#include<time.h>
#include<error.h>

#define SHOWHOST        //目前有个问题就是不清楚这个预处理是干什么的呀
void show(struct utmp *);

int main() {
    struct utmp current;
    int fd;
    int reclen = sizeof(current);
    if (fd = open(UTMP_FILE, O_RDONLY) == -1) {
        printf("error");
        exit(1);
    }
    //每次都从文件中读取相应大小的字节数放入缓冲当中
    while (read(fd, &current, sizeof(current)) == reclen) {
        show(&current);
    }
    close(fd);
    return 0;
}

void show(struct utmp *current) {
    //都是有指定宽度的
    printf("%-8.8s", current->ut_name);
    printf(" ");
    printf("%-8.8s", current->ut_line);
    printf(" ");
    printf("%10ld", current->ut_time);
    printf(" ");
    #ifdef SHOWHOST
    printf("(%s)", current->ut_host);
    #endif
    printf("\n");
}
//按理来说应该会有空白记录以及按照时区来的时间