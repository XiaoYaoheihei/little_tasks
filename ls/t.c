#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdlib.h>

ino_t get_node (char *fname);
int chdir(const char *path);

int main() {
    ino_t cur = get_node(".");
    printf(". is %ld\n\n", cur);
    int result = chdir("..");
    if (result == -1) {
        exit(1);
    } else {
        cur = get_node(".");
        printf(". is %ld\n\n", cur);
    }
    return 0;
}

ino_t get_node (char *fname) {
    //通过文件名找到所属目录的inode
    struct stat info;//获取文件的所有相关信息
    if (stat(fname, &info) == -1) {
        fprintf(stderr, "Cannot open this file");
        perror(fname);
        exit(1);
    }
    return info.st_ino;//成功获取当前目录文件的inode
}