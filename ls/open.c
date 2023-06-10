#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
//#include "tlpi_hdr.h"
int main() {
    int fd = open("startup", O_RDONLY);
    if (fd == -1) {
        printf("open fail\n");
    }
    int fd1 = open("myfile", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd1 == -1) {
        printf("open fail");
    } else {
        printf("open sucess");
    }
    printf("hhhhh");
    return 0;
}