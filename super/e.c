//一个程序运行另一个程序
/*execvp函数：
第一个参数：程序名
第二个参数：程序的命令行参数，注意参数的最后一个元素必须是NULL;
此函数在环境变量PATH指定的路径中查找此程序名
*/
#include <stdio.h>
#include <unistd.h>
int main () {
    char *arglist[3];
    arglist[0] = "ls";
    arglist[1] = "-l";
    arglist[2] = 0;
    printf("***, about to exec ls -l\n");
    execvp("ls", arglist);
    printf("*** ls is done,bye\n");
    return 0;
}
//打印出来没有第二条打印消息！！！
//问题：内核将新程序载入到当前进程，替代当前进程的代码和数据。是从哪里开始替换的？这是个问题。
//环境变量在哪里设置？环境变量在哪里可以更改？