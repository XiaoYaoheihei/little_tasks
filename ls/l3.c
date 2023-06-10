#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
//实现ls -i
//显示每一个文件的inode值
void show_inode(char *);

int main(int argc, char **argv) {
    if (argc == 1) {
        //printf("only one demo\n\n");
        show_inode(".");           //传进去当前目录的地址
    } else {
        while (--argc) {        //访问你指定的目录的地址
            //printf("%s:\n ", * ++argv);
            show_inode(*++argv);
        }
    }
    return 0;
}

void show_inode(char *path) {
    int count = 0;
    DIR *dirp;
    chdir(path);
    struct dirent *temp;
    if ((dirp = opendir(path)) == NULL) {
        printf("error in this %s\n", path);
    } else {
        while ((temp = readdir(dirp)) != NULL) {
            if (strcmp(temp->d_name, ".") == 0 || strcmp(temp->d_name, "..") == 0) {
                continue;
            } 
            count++;
            printf("%7ld %s", temp->d_ino, temp->d_name);
            if (count % 5 == 0) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
    }
}