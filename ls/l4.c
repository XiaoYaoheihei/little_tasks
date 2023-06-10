#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<time.h>
// typedef struct timespec {
//     __time_t tv_sec;    //秒
//     __suseconds_t tv_usec;//纳秒
// }timerecord;

//实现ls -t
//按照文件修改时间由近及远
int cal (char *);
char **restore(char *, int );
void sort1(char *);
void sort_modificationfile(char **, int );

int main(int argc, char **argv) {
    if (argc == 1) {
        //printf("only one demo\n\n");
        sort1(".");           //传进去当前目录的地址
    } else {
        while (--argc) {        //访问你指定的目录的地址
            //printf("%s:\n ", * ++argv);
            sort1(*++argv);
        }
    }
    return 0;
}

void sort1(char *path) { //文件排序
    int counts = cal(path);
    char **filenames = restore(path, counts);
    sort_modificationfile(filenames, counts);
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

void sort_modificationfile(char **filenames, int counts) {
    if (!filenames) {
        perror("error of the filenames");
        exit(1);
    }
    struct stat info;
    time_t filetime[counts];//记录每个文件的时间先后
    //timerecord current[counts]; 
    for (int i = 0; i < counts; i++) {
        if (stat(filenames[i], &info) == -1) {
            printf("error to open the file info\n");
        }
        //current[i] = info.st_mtim;
        filetime[i] = info.st_mtim.tv_sec;
    }
    time_t filetime_tem; 
    for (int i = 0; i < counts - 1; i++ ) {
        for (int j = 0; j < counts - 1 - i; j++) {
            if (filetime[j] < filetime[j+1]) {
                char *temp = (char *)malloc(sizeof(char)* 4096);
                filetime_tem = filetime[j];
                strcpy(temp, filenames[j]);

                filetime[j] = filetime[j+1];
                strcpy(filenames[j], filenames[j+1]);

                filetime[j+1] = filetime_tem;
                strcpy(filenames[j+1], temp);
                free(temp);
            }
        }
    }
    for(int i = 0; i < counts; i++) {
        printf("%s\n", filenames[i]);
    }
}