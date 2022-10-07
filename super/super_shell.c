#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <readline/history.h>
#include <readline/readline.h>

#define MAX_CMD 10
#define MAX_LEN 100
#define BUFFSIZE 100
#define OUT 0
#define IN 1

#define CLOSE "\001\033[0m\002"                 // 关闭所有属性
#define BEGIN(x,y) "\001\033["#x";"#y"m\002"    // x: 背景，y: 前景
struct 
{
    char *argv[BUFFSIZE];
    char *in;
    char *out;
    /* data */
}cmd[16];


int argc;                       //命令参数
char *argv[MAX_CMD];            //处理好的命令
char COM[MAX_CMD][MAX_LEN];     //处理好的命令
char backupCommand[BUFFSIZE];   //备份命令


void my_signal();//信号函数设置
void parse(char * );//解析命令行
void make_cmd(int, char**);//处理命令行
int parse_pipe(char *, int);//管道命令的分析
void Pipe(char *);//管道命令的处理
void Back(char*);//处理&
void Reout(char *);//处理“>”
void Rein(char *);//处理“<”
void Replusout(char *);//处理“>>”
void make_cd(int argc);//实现cd的切换

int main() {
    my_signal();
    while (1) {
        char envir[BUFFSIZE];       //存放当前路径
        getcwd(envir, BUFFSIZE);    //获取当前路径
        printf(BEGIN(36, 36)"%s: "CLOSE, envir);//色彩显示当前路径
        //readline库的使用
        char *command = readline(BEGIN(33, 33)"XIAOYAO-SUPER-SHELL >"CLOSE);
        if (!command) {
            perror("readline\n");
        }
        add_history(command);       //存放历史命令
        write_history(command);     //写入历史命令
        parse(command);             //解析命令
        make_cmd(argc, argv);       //执行命令
        argc = 0;                   //为了重新读取命令
        free(command);              //释放空间，防止造成内存泄漏
    }
}

void my_signal() {//父进程屏蔽掉所有信号
    signal(SIGHUP, SIG_IGN);        //挂断信号
    signal(SIGINT, SIG_IGN);        //ctl-c，程序终止
    signal(SIGTTIN, SIG_IGN);       
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);       //ctl-d，进程终止
}

void parse(char * command) {//解析命令行
    //初始化argc和argv
    for (int i = 0; i < MAX_CMD; i++) {
        argv[i] = NULL;
        for (int j = 0; j < MAX_LEN; j++) {
            COM[i][j] = '\0';
        }
    }
    // printf("okok\n\n");
    argc = 0;
    memset(backupCommand,0,sizeof(backupCommand));
    strcpy(backupCommand, command);//备份命令

    int j = 0;
    int len = strlen(command);
    for (int i = 0; i < len; i++ ){
        if (command[i] != ' ') {
            COM[argc][j++] = command[i];
            //argv[argc][j++] = command[i];
        } else {
            if (j != 0) {
                COM[argc][j] = '\0';
                argc++;
                j = 0;
            }
        }
    }
    if (j != 0) {   //命令行末尾
        COM[argc][j] = '\0';
    }
    // printf("okok\n\n");
    /*处理__内置命令__*/
    argc = 0;
    int flg = OUT;
    for(int i = 0; command[i] != '\0'; i++){
        if(flg == OUT && !isspace(command[i])){
            flg = IN;
            argv[argc++] = command + i;
        } else if(flg == IN && isspace(command[i])){
            flg = OUT;
            command[i] = '\0';
        }
    }
    argv[argc] = NULL;
    // printf("okok\n\n");
}

void make_cmd(int argc, char **argv) {//处理命令
    
    char buf[1024];
    //需要实现的几条特殊命令
    //"| 命令"
    for (int j = 0; j < argc; j++) {
        if (strcmp(COM[j], "|") == 0) {
            strcpy(buf, backupCommand);
            Pipe(buf);
            return;
        }
    }
    //"> 命令"
    for(int j = 0;j < argc; j++){
        if(strcmp(COM[j], ">") == 0){
            strcpy(buf,backupCommand);
            Reout(buf);
            return;
        }
    }
    //"< 命令"
    for (int j = 0; j < argc; j++) {
        if (strcmp(COM[j], "<") == 0) {
            strcpy(buf, backupCommand);
            Rein(buf);
            return;
        }
    }
    //">> "
    for (int j = 0; j < argc; j++) {
        if (strcmp(COM[j], ">>") == 0) {
            strcpy(buf, backupCommand);
            Replusout(buf);
            return;
        }
    }
    //"&"
    for (int j = 0; j < argc; j++) {
        if (strcmp(COM[j], "&") == 0) {
            strcpy(buf, backupCommand);
            Back(buf);
            return;
        }
    }

    //识别shell内置命令
    if (strcmp(COM[0], "cd") == 0) {
        make_cd(argc);
    } else if (strcmp(COM[0], "history") == 0) {
        printf("history\n");
    } else if (strcmp(COM[0], "exit") == 0) {
        printf("--------------------good bye--------------------\n");
        exit(0);
    } else {//比如说是ls -a -l这种的
        pid_t pid = fork();
        switch (pid)
        {
        case -1:
            perror("fork");
            break;
        case 0://子进程执行任务
            execvp(argv[0], argv);
            perror("execvp");
            exit(1);
        default://父进程等待子进程退出
            int status;
            waitpid(pid, &status, 0);//等待任何组进程
            int err = WEXITSTATUS(status);//宏用来判断进程是否正常退出
            if (err) {
                perror("WEXITSTATUS");
            }
            break;
        }
    }
} 

int flag_in = 0;
int flag_out = 0;
//输入和输出重定向
int parse_pipe(char *buf, int number) {//管道及其他命令分析大法
    int n = 0;
    char *p = buf;
    cmd[number].in = NULL;
    cmd[number].out = NULL;
    while(*p != '\0') {         
        if (*p == ' ') {            //将字符串中所有的空格,替换成'\0',方便后续拆分字符串
            *p++ = '\0';
            continue;
        }
        //判断是否需要和输入输出重定向搭配使用
        if (*p == '<') {
            *p = '\0';
            flag_in = 1;
            while(*(++p) == ' ');
            cmd[number].in = p;
            continue;
        }
        if (*p == '>') {
            *p = '\0';
            flag_out = 1;
            while(*(++p) == ' ') {
                ;
            }
            cmd[number].out = p;
            continue;
        }
        //去除空格
        if(*p != ' ' && ((p == buf) || *(p-1) == '\0')){
            if(n < MAX_CMD){
                cmd[number].argv[n++] = p++;
                continue;
            }else{
                return -1;
            }
        }
        ++p;
    }
    if (n == 0) {
        return -1;
    }
    
    return 0;
}

void Pipe(char *buf) {//处理管道命令
    int i, j;
    int fd[16][2];
    //管道对应的fd用数组存放起来
    int cmdnum = 0;//命令行序号数，管道数量
    char *back;
    char *fenge = buf;
    int pipe_num = 0;
    for (int k = 0; k < MAX_LEN; k++) {
        if (buf[k] == '|') {
            pipe_num++;
        }
    }
    //strsep函数是用来分割字符串
    while((back = strsep(&fenge, "|"))) {
        flag_out = 0;
        flag_in = 0;
        if (parse_pipe(back, cmdnum++) < 0) {
            cmdnum--;
            break;
        }
        if (cmdnum == 17) {                   //16根管道最多支持17条命令
            break;
        }
    }

    for(i = 0; i < pipe_num; i++) {       //创建出了管道
        if (pipe(fd[i])) {
            perror("pipe");
        }
    }

    pid_t pid;
    for (i = 0; i <= pipe_num; i++) {     //有多少管道就创建多少进程（兄弟进程无限多）
        if ((pid = fork()) == 0) {
            break;
        }
    }

    if (pid == 0) {             //子进程的处理方式
        if (pipe_num != 0) {
            if (i == 0){ //第一个创建的子进程
            //管道的输入为标准输入
                dup2(fd[0][1], STDOUT_FILENO);
                close(fd[0][0]);

                for (j = 1; j < pipe_num; j++){
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
            } else if (i == pipe_num){ //最后一个创建的子进程
            //管道的输出为标准输出
                dup2(fd[i-1][0], STDIN_FILENO);
                close(fd[i-1][1]);

                for (j = 0; j < pipe_num - 1; j++){
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
            } else{
                //重定中间进程的标准输入至管道读端
                dup2(fd[i-1][0], STDIN_FILENO); 
                close(fd[i-1][1]);
                //重定中间进程的标准输出至管道写端
                dup2(fd[i][1], STDOUT_FILENO);
                close(fd[i][0]);

                for (j = 0; j < pipe_num; j++){ //关闭不使用的管道读写两端
                    if (j != i || j != (i - 1)){
                        close(fd[j][0]);
                        close(fd[j][1]);
                    }
                }
            }
        }
        //用户指定了输入重定向
        if(flag_in){
            int file_fd = open(cmd[i].in, O_RDONLY);
            dup2(file_fd, STDIN_FILENO);
        }
        //用户指定的输出重定向
        if(flag_out){
            int file_fd = open(cmd[i].out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(file_fd, STDOUT_FILENO);
        }


        execvp(cmd[i].argv[0], cmd[i].argv); //执行用户输入的命令
        perror("execvp");
    } else {                    //父进程的处理方式
        //关闭父进程管道读写两端，等待子进程退出
        for (i = 0; i < pipe_num; i++) {
            close(fd[i][0]);
            close(fd[i][1]);
        }
        for (int i = 0; i < cmdnum; i++) {
            wait(NULL);
        }
    }
}

void Back(char *buf) {//处理&
    char BackBuf[strlen(buf)];
    memset(BackBuf, 0, strlen(buf));
    //提取 & 前的命令
    for(int i = 0; i < strlen(buf); i++){
        BackBuf[i] = buf[i];
        if(buf[i] == '&'){
            BackBuf[i-1] = '\0';
            break;
        }
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
    }

    if(pid == 0){
        //FILE *freopen(const chat*pathname, const char*mode, FILE *stream);
        freopen("/dev/null", "w", stdout); 
        freopen("/dev/null", "r", stdin);
        signal(SIGCHLD, SIG_IGN);
        parse(BackBuf);         //重新解析字符串（需要过滤掉&）
        execvp(argv[0], argv);  //执行相应的程序
        perror("execvp");
    }else{
        wait(NULL);
    }
}

void Reout(char *buf) {//处理>
    char outfile[1024];
    memset(outfile, 0, BUFFSIZE);
    int renum = 0;
    for (int i = 0; buf[i]; i++) {
        if (buf[i] == '>') {
            renum++;
            break;
        }
    }
    if (renum != 1) {
        printf("error in Reout \n");
        exit(1);
    }
    //与分割好的命令逐一比较，确定重定向文件
    for (int i = 0; i < argc; i++) {
        if (strcmp(COM[i], ">") == 0) {
            if (i + 1 < argc) {
                strcpy(outfile, COM[i+1]);
                break;
            } else {
                printf("missing the outputfile\n");
                exit(1);
            }
        }
    }

    for (int j = 0; j < strlen(buf); j++) {
        if (buf[j] == '>') {
            buf[j - 1] = '\0';
            break;
        }
    }
    parse(buf);     //重定向符号后面是文件名称，所以需要重新解析命令,将后面的文件名舍去


    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
    }
    if (pid == 0) {
        int fd;
        fd = open(outfile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fd < 0) {
            perror("open");
        }
        //STDOUT_FILENO是文件描述符
        dup2(fd, STDOUT_FILENO);        //十分关键关键关键的一步（子进程中执行的任务）
        execvp(argv[0], argv);
        if (fd != STDOUT_FILENO) {
            close(fd);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        int err = WEXITSTATUS(status);
        if (err) {
             perror("WEXITSTATUS");
        }
    }
}

void Rein(char *buf) {//处理<
    char outfile[1024];
    memset(outfile, 0, BUFFSIZE);
    int renum = 0;
    for (int i = 0; buf[i]; i++) {
        if (buf[i] == '<') {
            renum++;
            break;
        }
    }
    if (renum != 1) {
        printf("error in Reout \n");
        exit(1);
    }

    for (int i = 0; i < argc; i++) {//与分割好的命令逐一比较，确定重定向文件
        if (strcmp(COM[i], "<") == 0) {
            if (i + 1 < argc) {
                strcpy(outfile, COM[i+1]);
                break;
            } else {
                printf("missing the outputfile\n");
                exit(1);
            }
        }
    }
    for (int j = 0; j < strlen(buf); j++) {
        if (buf[j] == '<') {
            buf[j - 1] = '\0';
            break;
        }
    }
    parse(buf);     //重定向符号后面是文件名称，所以需要重新解析命令


    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
    }
    if (pid == 0) {
        int fd;
        fd = open(outfile, O_RDONLY, S_IRUSR | S_IWUSR);
        if (fd < 0) {
            perror("open");
        }
        dup2(fd, STDIN_FILENO);        //关键一步
        execvp(argv[0], argv);
        if (fd != STDOUT_FILENO) {
            close(fd);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        int err = WEXITSTATUS(status);
        if (err) {
             perror("WEXITSTATUS");
        }
    }
}

void Replusout(char *buf) {//处理>>
    char outfile[1024];
    memset(outfile, 0, BUFFSIZE);
    int renum = 0;
    for (int i = 0; buf[i]; i++) {
        if (buf[i] == '>' && buf[i+1] == '>') {
            renum++;
            break;
        }
    }
    if (renum != 1) {
        printf("error in Reout \n");
        exit(1);
    }

    for (int i = 0; i < argc; i++) {//与分割好的命令逐一比较，确定重定向文件
        if (strcmp(COM[i], ">>") == 0) {
            if (i + 1 < argc) {
                strcpy(outfile, COM[i+1]);
                break;
            } else {
                printf("missing the outputfile\n");
                exit(1);
            }
        }
    }
    for (int j = 0; j < strlen(buf); j++) {
        if (buf[j] == '>' && buf[j+1] == '>') {
            buf[j - 1] = '\0';
            break;
        }
    }
    parse(buf);     //重定向符号后面是文件名称，所以需要重新解析命令


    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
    }
    if (pid == 0) {
        int fd;                     //这里记得必须添加上追加的属性
        fd = open(outfile, O_RDWR | O_APPEND | O_CREAT , S_IRUSR | S_IWUSR);
        if (fd < 0) {
            perror("open");
        }
        dup2(fd, STDOUT_FILENO);        //关键一步
        execvp(argv[0], argv);
        if (fd != STDOUT_FILENO) {
            close(fd);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        int err = WEXITSTATUS(status);
        if (err) {
            perror("WEXITSTATUS");
        }
    }
}

void make_cd(int argc) {//实现argc命令,还需要搞清楚cd-是干嘛的？？？
    printf("实现cd命令\n");
    int ret = 0;
    int flag1 = 0;
    int flag_now = 0;
    if (argc == 1) {
        return;
    } else {
        for (int i = 0; COM[1][i]; i++) {
            if (COM[1][i] == '-') {
                flag1 = 1;
            } 
            if (COM[1][i] == '~') {
                flag_now = 1;
            }
        }
        if (flag_now) {
            ret = chdir("/home/xiaoyao");
            if (ret == -1) {
                perror("chdir");
            }
        } else if (flag1) {
            printf("不知道有什么用处？");
        } else {
            ret = chdir(COM[1]); //实现任意路径
            if (ret == -1) {
                perror("chdir");
            }
        }
    }
    if (ret) {
        return;
    }
}
