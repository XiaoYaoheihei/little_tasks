#include "epoll.h"
#include "funcs.h"
using namespace leveldb;
extern std::map<int, int> inform;
std::map<std::string, int > online;
//子线程执行任务
void* work(void* argc) {
    
    int cfd = *(int*)argc;
    printf("子线程进行通信的描述符为%d\n", cfd);
    //创建一个epoll用于检测通信描述符的缓冲区内是否有内容
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //用来存放已经就绪的文件描述符的结构体数组
    std::string current, info; 
    std::string& info1 = info;//记录登录人的id貌似更好
    
    char *buff;
    while (1) {
        static int i = 0;
        std::cout << "     "<< cfd << "    " << "next function" << std::endl;
        int n = epoll_wait(epfd, evs, 1, -1);
        if (evs->events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {//开始判断
            //客户端被挂掉了
            std::cout << "客户端被挂掉了" << std::endl;
            epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, 0);
            auto s1 = inform.find(cfd);
            auto fd1 = s1->second;
            std::string now = "exit";
            sendMsg(fd1, now, strlen(now.c_str()));
            online.erase(info1);
            inform.erase(cfd);
            std::cout << cfd << "  " << fd1 << std::endl;
            close(cfd);
            close(fd1);
            break;
        }
        recvMsg1(cfd, &buff, i);
        current = buff;
        std::cout << "        " << current << std::endl;
        if (current == "Login") {//登陆的时候服务端
            int n = login(cfd, info1);
        } else if (current == "Sign_in") {//注册端该做的事情
            int n = sign_in(cfd, 1);
        } else if (current == "Logout") {//注销时该做的事情
            int n = logout(cfd, "nonono");
        } else if (current == "Quit") {//此用户退出登录状态了
            auto s1 = inform.find(cfd);
            auto fd1 = s1->second;
            std::string now = "exit";
            sendMsg(fd1, now, strlen(now.c_str()));
            int m = inform.erase(cfd);
            int n = online.erase(info1); 
            if (n == 1) {
                close(fd1);
                close(cfd);
                break;
            } else {
                // std::cout << "退出出现问题" << std::endl;
                break;
            }
        } else if (current == "Findfriends") { //查看好友信息
            int n = findfriends(cfd, info1);
        } else if (current == "Blip") {//删除好友时
            int n = blip(cfd, info1, 1);
        } else if (current == "Add") { //增加好友
            int n = add(cfd, info1, 1);
        } else if (current == "Findmember") {//查看好友增加消息
            int n = findinfo(cfd, info1);
        } else if (current == "Findgroup") {//查看群添加消息
            int n = findgroup(cfd, info1);
        } else if (current == "chat") {//和好友进行聊天
            int n = chat(cfd, info1);
        } else if (current == "history") {//查看好友历史记录
            int n = history(cfd, info1);
        } else if (current == "sheild") {//屏蔽好友
            int n = sheild(cfd, info1);
        } else if (current == "Findonline") {//查看好友在线信息
            int n = f_online(cfd, info1);
        } else if (current == "Sendfile" ) {//服务器接收文件
            int n = filemenu(cfd, info1);
        } else if (current == "Gcreat") {//群组的创建
            int n = gcreat(cfd, info1);
        } else if (current == "Addgroup") {//申请加群
            int n = gadd(cfd, info1);
        } else if (current == "Deletegroup") {//群组的解散
            int n = gdelete(cfd, info1);
        } else if (current == "Qiutgroup") {//申请退群
            int n = gquit(cfd, info1);
        } else if (current == "Findallgroup") {//查看已经加入的群聊
            int n = gfind(cfd, info1);
        } else if (current == "Addadmi") {//添加管理员
            int n = admiadd(cfd, info1);
        } else if (current == "Deleteadmi") {//删除管理员
            int n = admidelete(cfd, info1);
        } else if (current == "Deltemember") {//删除群成员
            int n = memberdelete(cfd, info1);
        } else if (current == "Chatgroup") {//群聊天
            int n = groupchat(cfd, info1);
        } else if (current == "Historygroup") {//群聊消息的查看
            int n = ghistory(cfd, info1);
        } else if (current == "Findallmember") {//查看群成员
            int n = gmember(cfd, info1);
        } else if (current == "SendfileGroup") {//群发送文件
            int n = group_file(cfd, info1);
        }
        i++;

    }
    std::cout << "此客户端进程寿命已尽" << std::endl;
    free(buff);
    return NULL;
}