//重定向到文件的操作
//who > useelist
//只是使用程序简单实现了这个目的，但是并没有参数提示
//创建了子进程之后，在子进程中执行了who这个程序，who一如既往的将输出写进“1”中，但此时的“1”连接的是
//创建的文件useelist，其实已经把内容写进了这个文件当中
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    int pid;
    int fd;
    printf("about to run who into a file\n");

    if ((pid = fork()) == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        close(1);
        fd = creat("useelist", 0644);
        execlp("who", "who", NULL);
        perror("execlp");
        exit(1);
    } 

    if (pid != 0) {
        wait(NULL);
        printf("Done running who,results in userlist\n");
    }
    return 0;
}

// int main(int args, char **argv) {
//     pid_t pid = fork();
//     if (pid == -1) {
//         perror("fork");
//         exit(1);
//     } else if (pid == 0) {
//         execvp(argv[0], argv);
//         exit(1);
//     } else {
//         wait(NULL);
//         printf("Done running who,results in userlist\n");
//     }
//     return 0;
// }