#ifndef _EPOLL_H
#define _EPOLL_H
#include<iostream>
    //服务端
    int setlisten(int lfd, unsigned short port);
    //客户端
    int connetToHost(int fd, const char *ip, unsigned short port);

    //共同使用
    int sendMsg(int cfd, std::string Msg, int len);//发送数据
    int createSocket();//创建套接字
    int writen(int fd, const char* msg, int size);//发送指定长度的字符串
    int recvMsg(int cfd, char** msg);//接收数据
    int recvMsg1(int cfd, char** msg, int number) ;
    int readn(int fd, char *buff, int size);//读取指定长度的字符串
    
    //服务端处理函数
    int analysis(char *buff, std::string& s11, std::string& s22);//服务端登陆时解析函数
    int analysis(char *buff, std::string& s11, std::string& s22, int number);//注册是解析函数
    int analysis(char *buff, std::string& s11);//注销时解析函数





#endif