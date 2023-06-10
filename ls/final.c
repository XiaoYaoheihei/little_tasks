#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>
#include<unistd.h>

//实现ls -l
char *uid_to_name(uid_t);
char *gid_to_name(gid_t);
void make_li(char []);
void get_stat(char *);
void show_finf(char *, struct stat*);
void change_mode(int st_mode, char *mode) ;
//实现ls -a
void make1_li(char []);
//实现ls -s
void get_blocks(char *);
void get_block(char *);
//实现ls -i
void show_inode(char *);
//实现ls -r
void reverse (char *);
void sort (char **, int );
int cal (char *);
char **restore(char *, int );
//实现ls -t
void sort1(char *);
void sort_modificationfile(char **, int );
//实现ls -R
void recursion(char *);


int main(int argc, char **argv) {
    if (argc == 1) {
        printf("This is not a good way");
    } else {
        int count = 2;
        // while (count <= argc) {
            ++argv; //argv你这个小bug啊，你真的害的我好惨，找了好久才发现你
                    //我永远记住你了
            if (strcmp(*argv, "-l") == 0) {//列出当前目录下文件的具体信息
                if (count == argc) {
                    make_li(".");
                } else {
                    while (argc > 2) {        //访问制定目录下的基本信息而且是有限制条件的哦
                    printf("%s:\n ", * ++argv);
                    make_li(*argv);
                    argc--;
                    }
                }
            } else if(strcmp(*argv, "-a") == 0) {//列出当前目录下的所有文件
                if (count == argc) {
                    make1_li(".");
                } else {
                    while (argc > 2) {        //访问你指定的目录的地址
                    printf("%s:\n ", * ++argv);
                    make1_li(*argv);
                    argc--;
                    }
                }
            } else if (strcmp(*argv, "-R") == 0) {      //用递归的方式写出每个目录下的文件
                                                                                                              
            } else if (strcmp(*argv, "-t") == 0) { //按照文件的修改时间
                if (count == argc) {
                    sort1(".");
                } else {
                    while (argc > 2) {        //访问你指定的目录的地址
                    sort1(*++argv);
                    argc--;
                    }
                }
            } else if (strcmp(*argv ,"-r") == 0) {//按照逆序排列每个文件
                if (count == argc) {
                    reverse(".");
                } else {
                    while (argc > 2) {        //访问你指定的目录的地址
                    reverse(*++argv);
                    argc--;
                    }
                }
            } else if (strcmp(*argv ,"-i") == 0) {//显示每个文件的inode
                if (count == argc) {
                    show_inode(".");
                } else {
                    while (argc > 2) {        //访问你指定的目录的地址
                    show_inode(*++argv);
                    argc--;
                    }
                }
            } else if (strcmp(*argv ,"-s") == 0) {//实现块大小的
                if (count == argc) {
                    get_blocks(".");
                } else {
                    while (argc > 2) {        //访问你指定的目录的地址
                    get_blocks(*++argv);
                    argc--;
                    }
                }
            }
        //     count++;
        // } 
    }
    return 0;
}
//ls -l
void make_li(char *dirpath) {
    DIR *dirp = opendir(dirpath);
    if (dirp == NULL) {
        printf("fail to open the dir\n");
    } else {
        struct dirent *content;
        while ((content = readdir(dirp)) != NULL) {
            get_stat(content->d_name);      //获取每个文件的信息
        }
    }
}
void get_stat(char *filename) {             //获取信息的实现细节
    struct stat inform;
    if (stat(filename, &inform) == -1) {
        printf(" %s faile to get\n", filename);
    } else {                                //调用成功的话就写出信息
        show_finf(filename, &inform);
    }
}
void show_finf(char *filename, struct stat* inform) {
    char mode[11];

    char *uid_to_name();
    char *gid_to_name();
    char *ctime();

    change_mode(inform ->st_mode, mode);    //把权限位转化成字符串
    printf("%s ", mode);
    printf("%ld ", (long) inform->st_nlink);//链接数
    printf("%-8s ", uid_to_name(inform->st_uid));//把uid转化成字符串
    printf("%-8s ", gid_to_name(inform->st_gid));//把gid转化成字符串
    printf("%ld ", inform->st_size);             //所占字节数量
    printf("%.12s ", 4 + ctime(&inform->st_mtim));//最后的修改时间
    printf("%s\n", filename);                    //文件名称
}
void change_mode(int st_mode, char mode[]) {
    strcpy( mode, "----------" );           //首先给全部默认成为-，找到匹配的更改就行，这块的会涉及到掩码的相关知识
    //文件类型                                                    
    if (S_ISDIR(st_mode)) {
        mode[0] = 'd';
    }
    if (S_ISCHR(st_mode)) {
        mode[0] = 'c';
    }
    if (S_ISBLK(st_mode)) {
        mode[0] = 'b';
    }
    //uesr权限位
    if (st_mode & S_IRUSR) {
        mode[1] = 'r';
    }
    if (st_mode & S_IWUSR) {
        mode[2] = 'w';
    }
    if (st_mode & S_IXUSR) {
        mode[3] = 'x';
    }
    //group权限位
    if (st_mode & S_IRGRP) {
        mode[4] = 'r';
    }
    if (st_mode & S_IWGRP) {
        mode[5] = 'w';
    }
    if (st_mode & S_IXGRP) {
        mode[6] = 'x';
    }
    //others权限位
    if (st_mode & S_IROTH) {
        mode[7] = 'r';
    }
    if (st_mode & S_IWOTH) {
        mode[8] = 'w';
    }
    if (st_mode & S_IXOTH) {
        mode[9] = 'x';
    }
}
char *uid_to_name(uid_t userid) {
    struct passwd *getpwuid();
    struct passwd *pw_ptr;
    static char numstr[10];

    if ((pw_ptr = getpwuid(userid)) == NULL) {
        sprintf(numstr, "%d", userid);
        return numstr;
    } else {
        return pw_ptr -> pw_name;
    }

}
char *gid_to_name(gid_t groupid) {
    struct group *getgrgid();
    struct group *grp_ptr;
    static char numstr[10];

    if((grp_ptr = getgrgid(groupid)) == NULL){
        sprintf(numstr, "%d", groupid);
        return numstr;
    }else{
        return grp_ptr -> gr_name;
    }
}
//ls -a
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
//ls -s
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
//ls -i
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
//ls -r
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
//ls -t
void sort1(char *path) { //文件排序
    int counts = cal(path);
    char **filenames = restore(path, counts);
    sort_modificationfile(filenames, counts);
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
//ls -R

