//客户端好友管理界面
#include "epoll.h"
#include "funcc.h"
using namespace std;
 
int friends(int fd, string& buff, int number) {
    //将登陆人的id信息传进来之后再序列化，第二个参数
    string choice;
    while (1) {
        cout << "       1.查看好友      " << endl;
        cout << "       2.删除好友      " << endl;
        cout << "       3.增加好友      " << endl;
        cout << "       4.好友聊天      " << endl;
        cout << "       5.聊天记录      " << endl;
        cout << "       6.屏蔽好友      " << endl;
        //cout << "       7.发送文件      " << endl;
        //cout << "       8.查看消息      " << endl;
        cout << "       7.查看好友在线   " << endl;
        cout << "       0.返回上一层     " << endl;
        cin >> choice;
        if (choice == "0") {
            break;
        } else if (choice == "1") {
            //查看好友呗
            int n = findfriends(fd);
            if (n == 1) {
                continue;
            } else {
                perror("findfriends");
                continue;
            }
        } else if (choice == "2") {
            //删除好友呗
            char *buf;
            int n = blip(fd);
            if (n == 1) {
                cout << "删除完成" << endl;
                continue;
            } else {
                continue;
            }
        } else if (choice == "3"){
            //增加好友,自己的id在服务器端记录下来
            int n = add(fd, 1, 2);
            if (n == 1) {
                cout << "添加的消息已经发送" << endl;
            } else {
                cout << "添加的消息未发送成功" << endl;
            }
            continue;
        } else if (choice == "4"){//和好友进行聊天
            int n = chatmenu(fd, buff);
        } else if (choice == "5") {//查看聊天记录
            int n = history(fd, buff);
        } else if (choice == "6") {//屏蔽好友聊天
            int n = shelid(fd, buff);
        } else if (choice == "7") {//查看好友在线消息
            int n = find_online(fd);
        }
    }
    return 1;
}

//客户端查看好友并将好友序列化
int findfriends(int fd) {
    string s = "Findfriends";
    sendMsg(fd, s, strlen(s.c_str()));
    char *b;
    sleep(0.1);
    recvMsg(fd, &b);
    string buff = b;
    vector<string> list;
    vector<string>& list1 = list;
    int flag;
    int  n = analyse(buff, list1);
    if ((n == 1)&&(list.size() != 0)) {//返回成功的话打印所有的好友名
        for(auto i : list) {
            cout << i << "\n";
        }//目前只是打印人名，还并未判断在线信息
        flag = 1;
    } else {
        flag = 0;
    }
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
}

//客户端删除好友
int blip(int fd) {
    string s = "Blip";
    sendMsg(fd, s, 4);//提醒服务器端准备就绪
    char *b;
    sleep(0.1);
    recvMsg(fd, &b);
    string buff = b;//记录着自己的所有好友
    cout << "请从下面好友中选择一名进行聊天" << endl;
    vector<string> list;
    vector<string>& list1 = list;
    analyse(buff, list1);//查看好友
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    string id;
    cout << "请输入你要删除的好友的id" << endl;
    while (1) {
        cin >> id;
        // if (id == "exit") {
        //     return 1;
        // }
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == s) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的好友id不正确，请重新输入" << endl;
            continue;
        }
    }
    sendMsg(fd, id, strlen(id.c_str()));
    sleep(0.1);
    recvMsg(fd, &b);
    int flag;
    if (strcmp("YES", b) == 0) {
        flag = 1;
    } else {
        flag = -1;
        printf("delete error\n");
    }
    free(b);
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
}

//客户端请求增加好友并同时发送两个人的id
int  add(int fd,int number, int n) {
    string s = "Add";
    sendMsg(fd, s, 3);//提醒服务器就绪
    string id;
    cout << "请输入你要添加的人的id" << endl;
    cin >> id;
    sendMsg(fd, id, strlen(id.c_str()));
    char *buff1;
    sleep(0.1);
    recvMsg(fd, &buff1);
    int flag;
    if (strcmp("YES", buff1) == 0) {
        flag = 1;
    } else if(strcmp("NO", buff1) == 0){
        flag = 0;
        perror("server error");
    }
    free(buff1);
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
}
//好友聊天界面
int chatmenu (int cfd, string& useid) {
    string choice;
    while (1) {
        cout << "       0.返回上一层    " << endl;
        cout << "       1.聊天         " << endl;
        cout << "       2.发送文件      " << endl;
        cout << "       3.接收文件      " << endl;
        cin >> choice;
        if (choice == "1") {
            int n = chat(cfd, useid);
        } else if (choice == "2") {
            int n = sendfile(cfd);
        } else if (choice == "3") {
            int n = recvfile(cfd);
        } else if (choice == "0"){
            break; 
        } else {
            continue;
        }
    }
    return 1;
}

//发送文件
int sendfile(int cfd) {
    string a1 = "Sendfile";
    sendMsg(cfd, a1, strlen(a1.c_str()));
    info file;
    int size = 0;
    int sum = 0;
    struct stat sta;//文件属性信息
    sleep(0.1);
    char *b;
    recvMsg(cfd, &b);
    string buff = b;//记录着自己的所有好友
    free(b);
    cout << "请从下面好友中选择一名" << endl;
    vector<string> list;
    vector<string>& list1 = list;
    analyse(buff, list1);//查看好友
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    string s;
    cout << "请输入发送对象的id" << endl;
    cin >> s;
    sendMsg(cfd, s, strlen(s.c_str()));
    //按照常理来说这里的文件名字还是应该再判断一次的
    cout << "请输入文件名字" << endl;
    char filename[20];
    scanf("%s", filename);
    for(int j = 0; j < strlen(filename); j++) {
        file.file_name[j] = filename[j];
    }
    cout << "请输入文件的绝对路径" << endl;
    char path[20];
    while (1) {
        scanf("%s", path);
        if (stat(path, &sta) == -1) {
            perror("获取文件信息失败");
            perror("请重新输入文件绝对路径");
            continue;
        } else {
            break;
        }
    }
   
    file.file_length = sta.st_size;//记录长度
    //发送文件基本信息
    write(cfd, &file, sizeof(struct file_information));
    char buffer[1024];
    int filefd = open(path, O_RDWR);
    if (filefd == -1) {
        perror("open");
        return -1;
    }
    int flag = 0;
    while (1) {//开始循环读文件里面的内容
        size = read(filefd, buffer, sizeof(buffer));
        if (size == -1) {
            printf("文件传输失败");
            close(filefd);
            flag = -1;
            break;
        } else if (size > 0) {
            size = write(cfd, buffer, size);
            // sum += size;
            // unsigned long int p = (sum * 100)/file.file_length;
			// printf("\r文件已传输%ld%%\n",p);
        } else {
            printf("传输成功\n");
            close(filefd);
            flag = 1;
            break;
        }

    }
    if (flag == 1) {
        return 1;
    }
    return -1;
}

//客户端接收文件
int recvfile(int cfd) {
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    string path;
    cout << "请输入你要接收的文件目录地址" << endl;
    while (1) {
        cin >> path;
        DIR *dirp = opendir(path.c_str());
        if (dirp == NULL) { 
            perror("opendir");
            cout << "你输入的接收地址本机不存在，请重新输入文件目录地址" << endl;
            continue;
        } else {
            cout << "请等待好友的文件发送。。。" << endl;
            break;
        }
    }
    epoll_wait(epfd, evs, 1, -1);
    
    info file;
    int len = read(cfd, &file, sizeof(struct file_information));
    if (len <= 0) {
        cout << "接收文件基本信息失败" << endl;
        perror("read");
        return -1;
    }
    string filename = file.file_name;
    auto filesize = file.file_length;
    long long size = 0;
    long long recvsize = 0;
    //客户端本地新建的文件地址
    
    string filepath = path  + "/" + filename;
    int filefd = open(filepath.c_str(), O_RDWR | O_CREAT | O_APPEND, 0644);
    if (filefd == -1) {
        perror("open");
        return -1;
    }
    cout << "开始接收" << endl;
    char buffer[1024];
    long long re = 0;
    while (1) {
        // char *buffer;
        //直接从内核缓冲区里面读取信息
        // size = recvMsg(cfd, &buffer);
        size = read(cfd, buffer, sizeof(buffer));
        re += size;
        cout << "size = " << size << endl;
        if (size == -1) {
            if (errno == EINTR || errno ==  EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
        }  
        if (size > 0) {
            long long ret = write(filefd, buffer, size);//文件里面书写
            recvsize += ret;
            cout << "ret =" << ret << endl;
            memset(buffer, 0, sizeof(buffer));
            cout << filesize<< "\n" << re << "\n" << recvsize << endl;
            if (recvsize == filesize) {
                // free(buffer);
                cout << filesize << endl;
                cout << recvsize << endl;
                break;
            }
        }
        // free(buffer);
    }
    cout << "接收完毕" << endl;
    return 1;
}

//与好友进行聊天
int chat(int cfd, string& useid) {
    string a1 = "chat";
    sendMsg(cfd, a1, strlen(a1.c_str()));
    char *b;
    sleep(0.1);
    recvMsg(cfd, &b);
    string buff = b;//记录着自己的所有好友
    
    cout << "请从下面好友中选择一名进行聊天" << endl;
    vector<string> list;
    vector<string>& list1 = list;
    analyse(buff, list1);//查看好友
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    string s;
    cout << "请输入好友的id" << endl;
    while (1) {
        cin >> s;
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == s) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的好友id不正确，请重新输入" << endl;
            continue;
        }
    }

    sendMsg(cfd, s, strlen(s.c_str()));
    sleep(0.1);
    recvMsg(cfd, &b);
    if (strcmp(b, "好友已经屏蔽,无法聊天") == 0) {
        printf("%s\n", b);
        return 1;
    }
    cout << "请你们开始聊天吧" << endl;
    free(b);
    //下面直接开始聊天
        pthread_t tid;
        pthread_create(&tid, NULL, Sen, &cfd);//发送消息
        pthread_t tid1;
        pthread_create(&tid1, NULL, Re, &cfd);//接收消息
        //这里千万不敢用detach，吓死
        pthread_join(tid, NULL);
        pthread_join(tid1, NULL);
    return 1;
}
void *Sen(void* argc) {//不停地发送消息
    int *n = (int *)argc;
    string chating;
    while (1) {
        getline(cin, chating);
        //cin >> chating;
        sendMsg(*n, chating, strlen(chating.c_str()));
        if (chating == "exit") {
            break;
        }
    }
    return NULL;
}
void *Re(void* argc) {//不停地接收消息
    int *n = (int *)argc;
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = *n;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, *n, &ev);
    struct epoll_event evs[1];
    char *buf;
    while (1) {
        epoll_wait(epfd, evs, 1, -1);
        recvMsg(*n, &buf);
        if (strcmp("exit", buf) == 0) {
            break;
        }//自己退出程序的具体过程
        string temp = buf;
        cout << temp << endl;
    }
    free(buf);
    return NULL;
}

//查看历史聊天记录
int history(int cfd, string &buff) {
    string s = "history";
    sendMsg(cfd, s, strlen(s.c_str()));
    char *buf;
    sleep(0.1);
    recvMsg(cfd, &buf);
    string b = buf;
    cout << "请选择一位你要查看历史记录的好友" << endl;
    vector<string> list;
    analyse(b, list);//查看好友时的序列化
    for(auto i : list) {
        cout << i << "\n";
    }
    cout << endl;
    string useid1;
    cout << "请输入你要查看的好友id" << endl;
     while (1) {
        cin >> useid1;
        // if (useid1 == "exit") {
        //     return 1;
        // }
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == useid1) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的好友id不正确，请重新输入" << endl;
            continue;
        }
    }
    string now = buff + "-" + useid1;
    sendMsg(cfd, now, strlen(now.c_str()));
    sleep(0.1);
    recvMsg(cfd, &buf);
    string his = buf;//打印历史消息记录
    cout << his << endl;
    free(buf);
    return 1;
}

//屏蔽好友
int shelid(int cfd, string& info) {
    string s = "sheild";
    sendMsg(cfd, s, strlen(s.c_str()));
    char *b;
    sleep(0.1);
    recvMsg(cfd, &b);
    string buff = b;//记录着自己的所有好友
    
    cout << "请从下面好友中选择一名进行屏蔽" << endl;
    vector<string> list;
    vector<string>& list1 = list;
    analyse(buff, list1);//查看好友
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    cout << "请输入你要屏蔽的id" << endl;
    string useid;
    while (1) {
        cin >> useid;
        // if (useid == "exit") {
        //     return 1;
        // }
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == useid) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的好友id不正确，请重新输入" << endl;
            continue;
        }
    }
    sendMsg(cfd, useid, strlen(useid.c_str()));
    sleep(0.1);
    recvMsg(cfd, &b);
    printf("%s\n", b);
    free(b);
    return 1;
}

//查看好友在线信息
int find_online(int cfd) {
    string s = "Findonline";
    sendMsg(cfd, s, strlen(s.c_str()));
    char *b;
    sleep(0.1);
    recvMsg(cfd, &b);
    string buff = b;//记录着自己的所有好友
    
    cout << "请从下面好友中选择一名进行聊天" << endl;
    vector<string> list;
    vector<string>& list1 = list;
    analyse(buff, list1);//查看好友
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    cout << "请输入你要查看的好友id" << endl;
    string useid;
    while (1) {
        cin >> useid;
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == useid) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的好友id不正确，请重新输入" << endl;
            continue;
        }
    }
    sendMsg(cfd, useid, strlen(useid.c_str()));
    sleep(0.1);
    recvMsg(cfd, &b);
    string now = b;
    free(b);
    cout << now << endl;
    return 1;
}
