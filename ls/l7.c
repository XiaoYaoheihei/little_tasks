#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
//实现ls -s

void get_blocks(char *);
void get_block(char *);

int main(int argc, char **argv) {
    if (argc == 1) {
        //printf("only one demo\n\n");
        get_blocks(".");           //传进去当前目录的地址
    } else {
        while (--argc) {        //访问你指定的目录的地址
            //printf("%s:\n ", * ++argv);
            get_blocks(*++argv);
        }
    }
    return 0;
}

void get_blocks(char *path) {
    DIR *dirp;
    chdir(path);
    struct dirent *temp;
    if ((dirp = opendir(path)) == NULL) {
        printf("error in this %s\n", path);
    } else {
        while ((temp = readdir(dirp)) != NULL) {
            //printf("%lu\n", temp->st_blocks);
            get_block(temp->d_name);
        }
    }
}

void get_block(char *filename) {
    int count = 0;
    struct stat inform;
    if (stat(filename, &inform) == -1) {
        printf("error to get info");
    } else {
        count++;
        printf("%lu %s\n", inform.st_blocks, filename);
        if (count % 5 == 0 ) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
}
