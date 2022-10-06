//客户端查看添加消息
#include"epoll.h"
#include"funcc.h"

using namespace std;
//查看好友添加消息
int messagemember(int cfd, string& buff, string a1) {
    string s = "Findmember";
    sendMsg(cfd, s, strlen(s.c_str()));
    //创建一个epoll用于检测通信描述符的缓冲区内是否有内容
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //用来存放已经就绪的文件描述符的结构体数组
    while(1) {
        char *buf;
        epoll_wait(epfd, evs, 1, -1);
        int len = recvMsg(cfd, &buf);
        string now = buf;
        free(buf);
        if (now == "END") {
            break;
        }
        cout << now << endl;
        string attitude;
        cout << "YES/NO" << endl;
        cin >> attitude;
        sendMsg(cfd, attitude, strlen(attitude.c_str()));
    }
    cout << "信息处理完毕" << endl;
    cout << "请按任意建返回上一层" << endl;
    return 1;
}

//查看群添加消息
int messagegroup(int cfd, string& useid) {
    string s = "Findgroup";
    sendMsg(cfd, s, strlen(s.c_str()));
    //创建一个epoll用于检测通信描述符的缓冲区内是否有内容
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //用来存放已经就绪的文件描述符的结构体数组
    while(1) {
        char *buf;
        epoll_wait(epfd, evs, 1, -1);
        int len = recvMsg(cfd, &buf);
        string now = buf;
        free(buf);
        if (now == "END") {
            break;
        }
        cout << now << endl;
        string attitude;
        cout << "YES/NO" << endl;
        cin >> attitude;
        sendMsg(cfd, attitude, strlen(attitude.c_str()));
    }
    cout << "信息处理完毕" << endl;
    return 1;
}