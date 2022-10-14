#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main (int argc, char *argv[]) {
    if (argc < 2) {
        printf("忘记传递参数了\n");
    } else {
        int n = atoi(*++argv);
        sleep(n);
    }
    exit(0);
}

// int atoi(const char *s) {
//     int n = 0;
//     while ('0' <= *s && *s <= '9') {
//         n = n*10 + *s++ - '0';
//     }
//     return n;
// }