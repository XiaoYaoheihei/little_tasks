#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include "epoll.h"

//创建套接字
int createSocket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(0);
    }
    printf("创建成功\n");
    return fd;
}

//绑定本定端口并且设置监听
int setlisten(int lfd, unsigned short port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;  //地址族协议
    addr.sin_port = htons(port);//绑定的端口，一般5000以下的都归系统使用了
    //这里的端口可以依据情况而定
    addr.sin_addr.s_addr = INADDR_ANY;//绑定的本地的IP地址
    
    // 设置端口复用
    // 设置地址重用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        perror("bind");
        //exit(0);
        printf("绑定失败\n");
        return -1;
    }
    printf("绑定成功!\n");

    ret = listen(lfd, 128);
    if (ret == -1) {
        perror("listen");
        printf("监听失败\n");
        return -1;
    }
    printf("设置监听成功\n");
    
    return ret;
}

//客户端连接服务器
int connetToHost(int fd, const char *ip, unsigned short port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);   //端口转换成大端端口
    inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);
    //ip地址转化成大端

    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        perror("connect");
        return -1;
        //exit(0);
    }
    printf("成功与服务器建立连接。。。\n");
    return ret;

}
//接收指定字节数的字符串
int readn(int fd, char* buf, int size) {
    char *p = buf;          //辅助指针记录位置
    int count = size;
    while (count > 0) {
        int len = recv(fd, p, count, 0);
        if (len == -1) {
            if (errno == EINTR || errno ==  EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            perror("recve");
            return -1;
        } else if (len == 0) {//发送端端开连接
            return size - count ;//返回总共读到的字节数
        } else {
            p += len;      //有效内存处继续接收数据
            count -= len;   //还有多少没有接收
        }
    }
    return size;
}
//
int recvMsg1(int cfd, char** msg, int number) {
    //第二个参数是一个传出参数
    //char head[4];
    int len = 0;
    readn(cfd, (char *)&len, 4);//先读取前面4个字节数
    int count = ntohl(len);//网络字节序转换成主机字节序
    printf("%d:第%d次要接收的数据块的大小是:%d\n", cfd, number, count);
    char* data = (char *)malloc(count + 1);
    int length = readn(cfd, data, count);
    if (length != count) {
        printf("接收数据失败\n");
        printf("%d->%d    %d\n", cfd, length, count);
        close(cfd);
        free(data);
        return -1;
    } 

    data[count] = '\0';
    *msg = data;
    return length; 
}
//单纯地接受数据
int recvMsg(int cfd, char** msg) {
    //第二个参数是一个传出参数
    //char head[4];
    int len = 0;
    readn(cfd, (char *)&len, 4);//先读取前面4个字节数
    int count = ntohl(len);//网络字节序转换成主机字节序
    printf("要接收的数据块的大小是:%d\n", count);
    char* data = (char *)malloc(count + 1);
    int length = readn(cfd, data, count);
    if (length != count) {
        printf("接收数据失败\n");
        printf("%d->%d    %d\n", cfd, length, count);
        close(cfd);
        free(data);
        return -1;
    } 

    data[count] = '\0';
    // printf("%s\n", data);
    *msg = data;
    return length; 
}

//发送指定长度size的字符串
int writen (int cfd, const char* msg, int size) {
    const char* buf = msg;
    int current = size;
    while (current > 0) {
        int len = send(cfd, buf, size, 0);
        //往对应的写缓冲区里面写内容，但是缓冲区内容有限
        //返回实际发送的字节数
        if (len == -1) {
            close(cfd);
            return -1;
        } else if (len == 0) {
            continue;       //重新发送
        } else {
            buf += len;     //指针偏移
            current -= len; //剩余的字节数量
        }
    }
    return size;
}

//发送数据
int sendMsg(int cfd, std::string Msg, int len ) {
    if (cfd < 0 || Msg.size() < 0 || len <= 0) {
        return -1;
    }
    //将string类型的数据首先转化成指针类型
    const char* msg = Msg.c_str();
    char* data = (char *)malloc(len + 4);
    int biglen = htonl(len);
    memcpy(data, &biglen, 4);
    //printf("%d", data[0]);
    //内存拷贝，连续的4个字节数拷贝到内存中去
    //这四个字节数是包头
    //printf("cheng gong\n");
    memcpy(data + 4, msg, len);
    //将需要发送的内容进行拷贝
    int ret = writen(cfd, data, len + 4);
    if (ret == -1) {
        //perror("send\n");
        close(cfd);
    }
    return ret;
}


