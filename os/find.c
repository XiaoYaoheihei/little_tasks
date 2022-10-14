#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void find(char *path, const char *filename) {
    struct stat st;
    struct dirent de;
    int fd;
    char filebuf[512], *p;                      //存放文件名的buf
    char pathbuf[DIRSIZ];                       //路径buf
    if ((fd = open(path, 0)) < 0) {             //通过路径打开这个目录
        fprintf(2, "open error: %s", path);
        return;
    }
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {                     //为了防止有空目录出现
            continue;
        }
        strcpy(filebuf, de.name);
        if (stat(filebuf, &st) < 0) {
            printf("stat error: %s", filebuf);
        }
        switch (st.type) {
            case T_FILE:
                //本来想着调用函数，时间有开销算求
                // int n = judge(buf, filename, flag);
                // if (n == 1) {
                //     printf("%s\n", filename);
                // }
                if (strcmp(filebuf, filename) == 0) {
                    printf("%s/%s\n", path, filename);
                }
                break;
            case T_DIR:
                if (strcmp(filebuf, ".")!= 0 && strcmp(filebuf, "..") != 0) {
                    strcpy(pathbuf, path);          //复制path
                    p = pathbuf + strlen(pathbuf);  //指向path的末尾
                    *p++ = '/';                     //在末尾添加“/”
                    memmove(p, filebuf, DIRSIZ);    //拼接出新的路径
                    p[DIRSIZ] = '\0';
                    find(pathbuf, filename);        //递归查找
                }
                break;
        }
    }
    close(fd);
}
int main(int argc, char **argv) {
    if (argc < 3) {
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}

