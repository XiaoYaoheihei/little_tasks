#ifndef _FUNCS_H
#define _FUNCS_H

#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <leveldb/db.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
// #include <nlohmann/json.hpp>
#include "json.hpp"
#include <unistd.h>
#include <leveldb/db.h>
#include <pthread.h>
#include <fcntl.h>	
#include <string.h>
#include <errno.h>
#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <thread>

typedef struct file_information{
	char file_name[120];    //文件名字
	long file_length;//文件长度
} info;

extern leveldb::DB *db;//用户基本信息
extern leveldb::DB *db1;//好友聊天记录
extern leveldb::DB *db2;//群聊记录
//全局的map，用来标识通信和通知信息的
extern std::map<int, int> inform;
//全局的mutimap，用来存放群聊添加消息
extern std::multimap<std::string, std::string> groupinf;
//全局的multimap,用来存放好友添加消息
extern std::multimap<std::string, std::string> inf;
//全局的map,用来存放以及标志登录人的id以及fd
extern std::map<std::string, int > online;
//全局的map，用来存储以及标记登录人此时处于好友聊天界面，并得知聊天对象
extern std::map<int, std::string> flag_pos;
//全局的map，用来存储以及标记登录人此时处于群聊界面，并得知聊天对象
extern std::map<int , std::string> chat_pos;
//全局的map，存储此时是收文件还是发文件
extern std::map<int , int > file_pos;
//子线程处理函数
void* work(void* argc);


//注销
int logout(int , std::string );
//注册
int sign_in(int , int );
//登陆
int login(int , std::string& );
//查看好友信息
int findfriends(int , std::string& );
//删除好友
int blip(int , std::string& , int);
//增添，删除好友时的序列化
int analys(std::string& , std::string& );
//增加好友
int add(int , std::string& , int );
//查看增加信息情况
int findinfo(int , std::string&);
//查看群添加消息
int findgroup(int, std::string&);
//数据库中增加id信息
int add_base(std::string&, std::string&);
//往群聊消息数据库当中添加消息
int add_to_group(std::string& , std::string& );
//和好友聊天
int chat(int, std::string&);
//第二个数据库的k分割
void fenge(std::string, std::string&, std::string&);
//历史消息的存储
int store(std::string, std::string);
//好友聊天查看历史记录
int history(int , std::string&);
//好友屏蔽
int sheild(int, std::string& );
//查看好友在线消息
int f_online(int , std::string& );
//群组的创建
int gcreat(int , std::string& );
//群主创建群的初始化
int g_analys(std::string, std::string& );
//群组的解散
int gdelete(int , std::string& );
//申请加入群聊
int gadd(int , std::string& );
//申请退群
int gquit(int , std::string& );
//查看已经加入的群聊
int gfind(int , std::string& );
//查看群成员
int gmember(int , std::string& );
//添加管理员
int admiadd(int , std::string& );
//删除管理员
int admidelete(int , std::string& );
 //删除群成员
int memberdelete(int , std::string& );
//群聊天
int groupchat(int , std::string& );
//群聊历史消息记录的存储
int storeinfo(std::string& , std::string& );
//群聊消息的查看
int ghistory(int , std::string& ); 
//文件主界面
int filemenu(int, std::string&);
//接收文件
int recvfile (int, std::string&, std::string&, std::string);
//群文件相关的
int group_file(int , std::string& );
//接收文件
int recvfile_group(int , std::string& , std::string& , std::string );


#endif