#ifndef _FUNCC_H
#define _FUNCC_H

#include<iostream>
#include<dirent.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<sys/epoll.h>
#include<pthread.h>
#include<fcntl.h>	
#include<sstream>
#include<vector>

typedef struct file_information{
	char file_name[120];    //文件名字
	unsigned long int file_length;//文件长度
} info;

int Login(int fd, std::string&);
int Sign_in(int fd, int);
int Logout(int fd, std::string example);
int serialize(std::string& useid1, std::string& password1, std::string& current) ; //登陆
int serialize1(std::string& useid1, std::string& password1, std::string& usename1, std::string& buff1);//注册
int serialize2(std::string& useid1, std::string& buff1); //注销
//登录主界面的函数
int enter(int, std::string&, int, int);
//实时消息的通知
void* inform(void * ); 
//进入好友界面
int friends(int , std::string& , int );
//好友界面的查看好友
int findfriends(int);
//好友界面的删除好友
int blip(int );  
//好友界面的增加好友         
int add (int , int, int );   
//好友聊天
int chat (int , std::string& );
//好友聊天时的接收消息
void *Sen(void *);
//好友聊天时发送消息
void *Re(void  *);
//查看历史消息记录
int history(int , std::string&);
//好友界面的屏蔽好友
int shelid(int, std::string&);
//好友聊天界面
int chatmenu(int , std::string&);
//发送文件
int sendfile(int );
//接收文件
int recvfile(int );
//好友界面的查看在线消息
int find_online(int);
//消息添加界面
int messageadd(int , std::string& , std::string );
//进入好友消息添加界面        
int messagemember(int , std::string& , std::string);
//群消息添加界面
int messagegroup(int , std::string&);
//客户端添加好友时的序列化
int analyse(std::string& , std::string& );
//客户端查看好友时的序列化
int analyse(std::string& , std::vector<std::string>& );
//进入群聊界面
int group(int , std::string& );
//群聊界面的群组创建
int creat_group(int );
//群组的解散
int delete_group(int );
//申请加群
int add_group(int , std::string& );
//申请退群
int quit_group(int ); 
//查看已经加入的群聊
int find_group(int ); 
//查看群成员
int find_member(int );
//添加管理员
int add_admi(int );
//删除管理员
int delete_admi(int );
//删除群成员
int delete_member(int );
//群聊天界面
int group_chatmenu(int );
//群发送文件
int group_sendfile(int );
//群接收文件
int group_recvfile(int );
//群聊天
int group_chat(int );
//客户端查看群组时的序列化
int group_analyse(std::string , std::vector<std::string> &);
//不停地发送消息
void *Seng(void* );
//不停地接收消息
void *Reg(void* );
//群聊消息的查看
int group_history(int );

#endif