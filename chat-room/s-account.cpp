//服务器账号管理
#include "funcs.h"
#include "epoll.h"
using namespace leveldb;
extern std::map<std::string, int > online;
//注销时候的服务器
int logout(int cfd, std::string example) {
    printf("服务端进行通信的描述符为%d\n", cfd);
    //注销的时候需要一个参数记录id
    char *buff;
    //创建一个epoll用于检测通信描述符的缓冲区内是否有内容
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //用来存放已经就绪的文件描述符的结构体数组
    epoll_wait(epfd, evs, 1, -1);
    int len = recvMsg(cfd, &buff);
    int flag = 0;
    std::string s1;//id
    std::string& s11 = s1;
    int n = analysis(buff, s11);
    if (n == 0) {
        std::cout << "analysis error" << std::endl;
    } else {
        std::cout << "analysis sucess" << std::endl;
    }
    if (len > 0) {
        std::cout << "开始在数据库中进行查找" << std::endl;
        std::cout << s1 << std::endl;
        Iterator* ita = db->NewIterator(ReadOptions());
        ita->SeekToFirst();
        while (ita-> Valid()) {
            Slice key = ita->key();
            if (key.ToString() == s1) {
                //删除掉这个相关的信息
                leveldb::Status status = db->Delete(leveldb::WriteOptions(), key);
                if (status.ok()) {
                    std::cout << "key: " << key.ToString() << " " << "删除成功。" << std::endl;
                }
                flag = 1;
                
                break;
            }
            ita->Next();
        }
        delete ita;
    }
    free(buff);
    std::string now;
    if (flag == 1) {
        now = "YES";
        sendMsg(cfd, now, 3);
        return 1;
    } else {
        now = "NO";
        sendMsg(cfd, now, 2);
        return -1;
    }
}

//注册时候的服务端
int sign_in(int cfd, int number) {
    printf("服务端进行通信的描述符为%d\n", cfd);
    //注册的时候需要两个参数
    //一个记录key，一个记录value
    char *buff;
    //创建一个epoll用于检测通信描述符的缓冲区内是否有内容
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //用来存放已经就绪的文件描述符的结构体数组
    epoll_wait(epfd, evs, 1, -1);

    int len = recvMsg(cfd, &buff);
    
    std::string s1;//key
    std::string s2;//value
    int n = analysis(buff, s1, s2, 1);
    if (n == 0) {
        std::cout << "analysis error" << std::endl;
    } else {
        std::cout << "analysis sucess" << std::endl;
    }

    int flag = 0;
    std::string now;//用来判断是否添加成功的信息
    leveldb::Slice key = s1;
    leveldb::Status status = db->Put(leveldb::WriteOptions(), key, s2);
    if(status.ok()) {
        now = "YES";
        sendMsg(cfd, now, 3);
        flag = 1;
    } else {
        now = "NO";
        sendMsg(cfd, now, 2);
        flag = 0;
    }
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
}

//登陆的时候服务端应该做的事情
int login(int cfd, std::string& info1) {
    printf("服务端进行通信的描述符为%d\n", cfd);
    //我想的是一次性把账号和密码读完
    //开始通过缓冲区内部的账号和密码在数据库中查找信息
    //如果在数据库中查找到了匹配的信息
    //将查找到的结果返回给客户端
    char *buff;
    int len;
    
    //创建一个epoll用于检测通信描述符的缓冲区内是否有内容
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //用来存放已经就绪的文件描述符的结构体数组
    epoll_wait(epfd, evs, 1, -1);
    
    len = recvMsg(cfd, &buff);
    //解析接收过来的字符串并且在数据库中进行查找
    //首先把字符串转化成为josn格式
    //我想的是通过josn格式然后把字符串提取出来
    std::string s1;//默认id
    std::string s2;//默认password
    //在解析的时候同时进行赋值判断
    int n = analysis(buff, s1, s2);
    if (n == 0) {
        std::cout << "analysis error" << std::endl;
    } else {
        std::cout << "analysis sucess" << std::endl;
    }
    int flag = 0;
    std::string now;//用来判断查找成功的信息
    if (len > 0) {
        //std::cout << "开始在数据库中进行查找" << std::endl;
        //有一个记录的临时变量
        Slice s3;
        std::string s5;
        Iterator* ita = db->NewIterator(ReadOptions());
        ita->SeekToFirst();
        while (ita-> Valid()) {
            Slice key = ita->key();
            if (key.ToString() == s1) {
                s3 = ita->value();//首先转化成string类型
                s5 = s3.ToString();
                auto s4 = nlohmann::ordered_json::parse(s5);
                if (s4["password"].get<std::string>() == s2) {
                    flag = 1;
                    info1 = s1;
                    break;
                }
            }
            ita->Next();
        }
        delete ita;
        free(buff);
        
        if (flag == 1) {
            std::cout << "存在此账号" << std::endl;
            if (online.count(s1) == 0) {//没有人正在登录中
                now = "YES";
                online.insert({s1, cfd});//登录人加到在线者map里面去
            } else {//有人正在登录
                now = "Already online";
            }
            //判断密码是不是正确的      
            //发送一个检测的标志信息
            sendMsg(cfd, now, strlen(now.c_str()));     
        } else {
            std::cout << "不存在此账号" << std::endl;
            now = "NO";
            sendMsg(cfd, now, 3);
        }
        flag = 1;
    } else if (len == 0) {
        std::cout << "客户端已经断开了连接" << std::endl;
        flag = -1;
    } else if (len == -1) {
        perror("recv");
    }
    if (flag == 1) {
        return 1;
    } else if (flag == -1) {
        return -1;
    } else {
        exit(0);
    }
}
