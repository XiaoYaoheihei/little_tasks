#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<string.h>
//实现ls -a

void make1_li(char []);

int main(int argc, char **argv) {
    if (argc == 1) {
        //printf("only one demo\n\n");
        make1_li(".");           //传进去当前目录的地址
    } else {
        while (--argc) {        //访问你指定的目录的地址
            printf("%s:\n ", * ++argv);
            make1_li(*argv);
        }
    }
    return 0;
}

void make1_li(char *dirpath) {
    int count = 0;
    DIR *dirp = opendir(dirpath);
    if (dirp == NULL) {
        printf("fail to open the dir\n");
    } else {
        struct dirent *content;
        while ((content = readdir(dirp)) != NULL) {
            count++;
            printf("%s\n",content->d_name);
            // if (count % 5 == 0) {
            //     printf("\n");
            // } else {
            //     printf(" ");
            // }
        }
    }
    int flag = closedir(dirp);
    if (flag == 1) {
        printf("fail to close\n");
    } else {
        printf("success to close\n");
    }
}