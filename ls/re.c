//递归列出当前目录下的所有文件
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    if (argc == 1) {
        make(".");
    }

}