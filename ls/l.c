#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<string.h>

//实现ls的功能
//如果要实现ls -a有两个地方：
//隐藏文件问题，把本目录和上一层目录下出来
void make_li(char []);

int main(int argc, char **argv) {
    if (argc == 1) {
        //printf("only one demo\n\n");
        make_li(".");           //传进去当前目录的地址
    } else {
        while (--argc) {        //访问你指定的目录的地址
            printf("%s:\n ", * ++argv);
            make_li(*argv);
        }
    }
    return 0;
}

void make_li(char *dirpath) {
    int count = 0;
    printf("hhhh\n");
    DIR *dirp = opendir(dirpath);
    if (dirp == NULL) {
        printf("fail to open the dir\n");
    } else {
        struct dirent *content;
        while ((content = readdir(dirp)) != NULL) {
           if (strcmp(content->d_name, ".") == 0 || strcmp(content->d_name, "..") == 0) {
                continue;
            } 
            count++;
            printf("%s",content->d_name);
            if (count % 5 == 0) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
    }
    int flag = closedir(dirp);
    if (flag == 1) {
        printf("fail to close\n");
    } else {
        printf("success to close\n");
    }
}