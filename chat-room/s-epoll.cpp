
#include "epoll.h"
#include "funcs.h"
using namespace std;
leveldb::DB *db;//用户基本信息
leveldb::DB *db1;//好友聊天记录
leveldb::DB *db2;//群聊记录
map<int, int> inform;//通信描述符和通知消息
int main () {
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "/home/xiaoyao/database", &db);
    if(status.ok()) {
        std::cout << status.ToString() << std::endl;
    } else {
        std::cout << "faile to open" << std::endl;
    }
    leveldb::Status status1 = leveldb::DB::Open(options, "/home/xiaoyao/database1", &db1);
    if(status1.ok()) {
        std::cout << status1.ToString() << std::endl;
    } else {
        std::cout << "faile to open" << std::endl;
    }
    leveldb::Status status2 = leveldb::DB::Open(options, "/home/xiaoyao/database2", &db2);
    if(status2.ok()) {
        std::cout << status2.ToString() << std::endl;
    } else {
        std::cout << "faile to open" << std::endl;
    }
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket errror");
        exit(1);
    }
    int ret = setlisten(lfd, 10000);
    if (ret == -1) {
        return -1;
    }
    //开始创建基本的epoll模型
    int epfd = epoll_create(1);
    if (epfd == -1) {
        perror("epoll_create");
        exit(0);
    }
    //向epoll中添加需要检测的节点
    //当前只有监听的文件描述符
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET ;    //检测读缓冲区是否有数据
    //ev.events = EPOLLIN;
    ev.data.fd = lfd;       //用联合体记录下来此文件描述，方便返回查看
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if (ret == -1) {
        perror("epoll_ctl");
        exit(0);
    }
    struct epoll_event evs[1024];
    //用来存放已经就绪的文件描述符的结构体数组
    int size = sizeof(evs)/sizeof(evs[0]);
    map<string, int> ip_pos;//定义了一个地址容器
    while (1) {
        //开始持续检测有哪些文件描述符就绪
        int num = epoll_wait(epfd, evs, size, -1);
        // cout << "num = " << num << "\n" << endl;
        //就绪的文件描述符的总个数
        for (int i = 0; i < num; i++) {
            int currfd = evs[i].data.fd;
            if (currfd == lfd) {//是监听的文件描述符
                struct sockaddr_in cliaddr;//把客户端的IP写到这个结构体里面
                unsigned int clilen = sizeof(cliaddr);
                //开始建立新的连接
                cout << "有客户端请求连接。。。" << "\n" << endl;
                int cfd = accept(currfd, (struct sockaddr*)&cliaddr, &clilen);
                printf("创建的通信描述符为%d\n", cfd);
                char ip[24] = {0};
                string ip_now = inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip));
                cout << ip_now << endl;
                if (ip_pos.count(ip_now) == 1) {
                    for (auto i = ip_pos.begin(); i != ip_pos.end(); i++) {
                        cout << i->first << "     " <<  i->second << endl;
                    }
                    //ip地址容器里面本身就有第一次的通信描述符
                    //那这一次建立的通信描述符就是只管接收消息的通信描述符
                    cout << "建立实时通信成功" << endl;
                    auto a1 = ip_pos.find(ip_now);
                    auto a2 = a1->second;
                    inform.insert({a2, cfd});
                    ip_pos.erase(ip_now);//用完之后就删除掉
                    continue;
                } else {
                    //ip地址容器里面没有建立的通信描述符
                    ip_pos.insert({ip_now, cfd});
                }
                //设置非阻塞属性
                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);
                //将新得到的通信描述符添加到epoll模型当中去
                //再次持续检测文件描述符的时候就可以检测到了
                //ev.events = EPOLLIN;
                ev.events = EPOLLIN | EPOLLET;
                //检测通信描述符的读缓冲区并且是以ET的形式
                ev.data.fd = cfd;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                if (ret == -1) {
                    perror("epoll_ctl-accept");
                    exit(0);
                }
            } else {
                pthread_t pth1;
                pthread_create(&pth1, NULL , work, &currfd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, currfd, NULL);
                pthread_t detach(pth1);
            }
        }
    }
    delete db;
    delete db1;
    delete db2;
    return 0;
}
