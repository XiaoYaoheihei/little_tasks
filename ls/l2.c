#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
//实现ls -R 递归实现对所有文件的遍历
void recursion(char *);

int main(int argc, char **argv) {
    if (argc == 1) {
        //printf("only one demo\n\n");
        recursion(".");           //传进去当前目录的地址
    } else {
        while (--argc) {        //访问你指定的目录的地址
            //printf("%s:\n ", * ++argv);
            recursion(*++argv);
        }
    }
    return 0;
}

void recursion(char *path) {
    printf("%s:\n", path);
    chdir(path);
    DIR *dirp;
    struct dirent *current;
    if ((dirp = opendir(path)) == NULL) {
        fprintf(stderr, "fail to open this %s\n", path);
        char buf[4000];
        getcwd(buf, sizeof(buf));
        printf("Can't open the directory now:%s\n", buf);
    } else {
        while ((current = readdir(dirp)) != NULL) {
            if (strcmp(current->d_name, ".") == 0 || strcmp(current->d_name, "..") == 0) {
                continue;
            } 
            struct stat info;
            char temp_dir[4096];
            sprintf(temp_dir, "/%s/%s:", path, current->d_name );
            stat(temp_dir , &info);
            if (S_ISDIR(info.st_mode)) { //判断此时的文件类型（只用判断是不是目录就行了）
                //printf("%s\n", current.d_name);
                recursion(temp_dir);
            } else {
                printf("%s\n", current->d_name);
            }
        }
    }
    return;
}