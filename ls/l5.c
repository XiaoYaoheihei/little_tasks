#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
//实现ls -r
//逆序排列
void reverse (char *path);
void sort (char **filenames, int counts);
int cal (char *path);
char **restore(char *path, int counts);

int main(int argc, char **argv) {
    if (argc == 1) {
        //printf("only one demo\n\n");
        reverse(".");           //传进去当前目录的地址
    } else {
        while (--argc) {        //访问你指定的目录的地址
            //printf("%s:\n ", * ++argv);
            reverse(*++argv);
        }
    }
    return 0;
}

void reverse (char *path) {
    DIR *dirp;
    struct dirent *temp;
    if ((dirp = opendir(path)) == NULL) {
        printf("erro in this directory");\
        exit(1);
    } else {
        int counts = cal(path);
        char **filename = restore(path, counts);
        sort(filename, counts);
    }
}

int cal (char *path) {//计算出来此目录下的文件个数
    int count = 0;
    DIR *dirp;
    chdir(path);
    struct dirent *temp;
    struct stat info;
    if ((dirp = opendir(path)) == NULL) {
        printf("error in this %s\n", path);
    } else {
        while ((temp = readdir(dirp)) != NULL) {
            if (stat(temp->d_name, &info) != -1) {
                if (S_ISREG(info.st_mode) || S_ISDIR(info.st_mode)) {
                    count++;
                }
            }
        }
    }
    return count;
}

char **restore(char *path, int counts) {
    char **filenames = (char **)malloc(sizeof(char*) *counts);
    DIR *dirp;
    chdir(path);
    struct dirent *temp;
    int count = 0;
    if ((dirp = opendir(path)) == NULL) {
        printf("error in this %s\n", path);
    } else {
        while ((temp = readdir(dirp)) != NULL && count < counts) {
            filenames[count] = (char *)malloc(sizeof(char)*strlen(temp->d_name));
            strcpy(filenames[count++], temp->d_name);
            }
        }
        return filenames;
    }

void sort (char **filenames, int counts) {
    char temp[4096];
    int count = 0;
    int i ;
    // while (i != j) { //字符串的交换
    //     strcpy(temp, filenames[j]);
    //     strcpy(filenames[j], filenames[i]);
    //     strcpy(filenames[i], temp);
    //     i++;
    //     j--;
    // }
    for (i = counts - 1; i >= 0; i--) {
        count++;
        printf("%s\n", filenames[i]);
        // if (count % 5 == 0) {
        //     printf("\n");
        // } else {
        //     printf(" ");
        // }
    }
}