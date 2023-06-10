#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>
//实现ls -l功能       
                     
char *uid_to_name(uid_t);
char *gid_to_name(gid_t);
void make_li(char []);
void get_stat(char *);
void show_finf(char *, struct stat*);
void change_mode(int st_mode, char *mode) ;

int main(int argc, char **argv) {
    if (argc == 1) {            //访问当前目录并列出当前目录下文件的基本信息
        make_li(".");
    } else {
        while (--argc) {        //访问制定目录下的基本信息
            printf("%s:\n ", * ++argv);
            make_li(*argv);
        }
    }
    return 0;
}

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