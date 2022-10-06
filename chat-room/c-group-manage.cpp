//客户端群管理
#include "epoll.h"
#include "funcc.h"
using namespace std;

int group(int cfd, string& buff) {
    //使用id去判断消息的可读性
    //是群主，管理员，聊天人员
    string choice;
    while (1) {
        cout << "       1.群组创建          " << endl;
        cout << "       2.群组解散          " << endl;
        cout << "       3.申请加群          " << endl;
        cout << "       4.申请退群          " << endl;
        cout << "       5.查看已加入群聊     " << endl;
        cout << "       6.查看群成员        " << endl;
        cout << "       7.添加管理员        " << endl;
        cout << "       8.删除管理员        " << endl;
        cout << "       9.删除群成员        " << endl;
        cout << "      10.群聊天            "<< endl;
        //cout << "      11.发送文件          " << endl;
        cout << "      11.查看历史记录       " << endl; 
        cout << "       0.返回上一层        " << endl;
        cin >> choice;
        if (choice == "0") {
            break;
        } else if (choice == "1") {
            int n = creat_group(cfd);
        } else if (choice == "2") {
            int n = delete_group(cfd);
        } else if (choice == "3") {
            int n = add_group(cfd, buff);
        } else if (choice == "4") {
            int n = quit_group(cfd);
        } else if (choice == "5") {
            int n = find_group(cfd);
        } else if (choice == "6") {
            int n = find_member(cfd);
        } else if (choice == "7") {
            int n = add_admi(cfd);
        } else if (choice == "8") {
            int n = delete_admi(cfd);
        } else if (choice == "9") {
            int n = delete_member(cfd);
        } else if (choice == "10") {
            int n = group_chatmenu(cfd);
        } else if (choice == "11") {
            int n = group_history(cfd);
        } else {
            continue;
        }
    }
    return 1;
}

//群组的创建
int creat_group(int cfd) {
    string now = "Gcreat";
    sendMsg(cfd, now, strlen(now.c_str()));
    cout << "请输入你要创建的群id(不得超过6位)" << endl;
    string gid;
    while (1) {
        cin >> gid;
        if (gid.size() > 6) {
            cout << "您输入了不正确的id，请重新输入" << endl;
            continue;
        } else {
            break;
        }
    }
    sendMsg(cfd, gid, strlen(gid.c_str()));
    sleep(0.1);
    char *temp;
    int flag = 0;
    recvMsg(cfd, &temp);
    if (strcmp(temp, "YES") == 0) {
        printf("%s\n创建成功", temp);
        flag = 1;
    } else {
        printf("%s\n创建失败", temp);
        flag = 0;
    }
    free(temp);
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
}

//群组的解散
int delete_group(int cfd) {
    string now = "Deletegroup";
    sendMsg(cfd, now, strlen(now.c_str()));
    sleep(1);
    char *temp;
    recvMsg(cfd, &temp);
    string id = temp;
    if (id == "无") {
        cout << "你还没有加入任何一个群" << endl;
        return 1;
    }
    cout << "请在以下群中选择你要解散的群id，并且输入此群id" << endl;
    vector<string> list;
    group_analyse(id, list);
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    string gid;
    cout << "请输入群聊id" << endl;
    while (1) {
        cin >> gid;
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == gid) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的群聊id不正确，请重新输入" << endl;
            continue;
        }
    }
    sendMsg(cfd, gid, strlen(gid.c_str()));
    sleep(1);
    recvMsg(cfd, &temp);
    int flag = 0;
    if (strcmp(temp, "你不具备权限") == 0) {
        cout << "你不具备权限" << endl;
        flag = -1;
    } else {
        cout << "解散成功" << endl;
        flag = 1;
    }
    free(temp);
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
    
}

//申请加群
int add_group(int cfd, string& useid) {
    string now = "Addgroup";
    sendMsg(cfd, now, strlen(now.c_str()));
    cout << "请输入你要加入的群id" << endl;
    string gid;
    while (1) {
        cin >> gid;
        if(gid.size() > 6) {
            cout << "你的id格式不正确，请重新输入" << endl;
            continue;
        } else {
            break;
        }
    }
    sendMsg(cfd, gid, strlen(gid.c_str()));
    sleep(0.1);
    char *temp;
    recvMsg(cfd, &temp);
    if (strcmp(temp, "YES") == 0) {
        cout << "恭喜你申请成功,等待相关人员同意" << endl;
    } else {
        cout << "不存在此群id，请重新申请" << endl;
    }
    free(temp);
    return 1;
}

//申请退群
int quit_group(int cfd) {
    string now = "Qiutgroup";
    sendMsg(cfd, now, strlen(now.c_str()));
    cout << "请在以下群中选择你要退出的群" << endl;
    char *temp;
    sleep(0.1);
    recvMsg(cfd, &temp);
    string results = temp;
    cout << results << endl;
    string gid;
    cin >> gid;
    sendMsg(cfd, gid, strlen(gid.c_str()));
    sleep(1);
    recvMsg(cfd, &temp);
    printf("%s\n", temp);//打印退群的结果
    free(temp);
    return 1;
}

//查看已经加入的群聊
int find_group(int cfd) {
    string now = "Findallgroup";
    sendMsg(cfd, now, strlen(now.c_str()));
    char *temp;
    sleep(0.1);
    recvMsg(cfd, &temp);
    string results = temp;
    cout << results << endl;
    free(temp);
    return 1;
}

//查看群成员
int find_member(int cfd) {
    string now = "Findallmember";
    sendMsg(cfd, now, strlen(now.c_str()));
    char *temp;
    sleep(0.1);
    recvMsg(cfd, &temp);
    string results = temp;
    if (results == "无") {
        cout << "无" << endl;
        return 1;
    }
    vector<string> list;
    group_analyse(results, list);
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    cout << "请选择你要查看的群id" << endl;
    string gid;
    while (1) {
        cin >> gid;
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == gid) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的群聊id不正确，请重新输入" << endl;
            continue;
        }
    }
    sendMsg(cfd, gid, strlen(gid.c_str()));
    sleep(0.5);
    recvMsg(cfd, &temp);
    results = temp;
    cout << results;
    free(temp);
    return 1;
}

//添加管理员
int add_admi(int cfd) {
    string now = "Addadmi";
    sendMsg(cfd, now, strlen(now.c_str()));
    char *temp;
    sleep(0.1);
    recvMsg(cfd, &temp);
    string results = temp;
    if (results == "无") {
        cout << "无" << endl;
        return 1;
    }
    vector<string> list;
    group_analyse(results, list);
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    cout << "请选择你要进行的群id" << endl;
    string gid;
    while (1) {
        cin >> gid;
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == gid) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的群聊id不正确，请重新输入" << endl;
            continue;
        }
    }
    sendMsg(cfd, gid, strlen(gid.c_str()));
    sleep(0.5);
    recvMsg(cfd, &temp);
    sleep(0.1);
    if (strcmp(temp , "你具备权限") == 0) {
        // cout << "jinlaile " << endl;
        sleep(0.1);
        recvMsg(cfd, &temp);
        printf("%s\n", temp);
        cout << "请在群成员中选择一位你要添加的群成员" << endl;
        string id;
        cin >> id;
        sendMsg(cfd, id, strlen(id.c_str()));
        sleep(0.5);
        recvMsg(cfd, &temp);
        if (strcmp(temp, "yes") == 0) {
            printf("恭喜你添加成功\n");
        } else {
            printf("添加失败,不存在此人\n");
        }
    } else {//不具备权限
        printf("%s\n", temp);
        cout << "请重新进入此页面进行选择" << endl;
    }
    free(temp);
    return 1;

}

//删除管理员
int delete_admi(int cfd) {
    string now = "Deleteadmi";
    sendMsg(cfd, now, strlen(now.c_str()));
    char *temp;
    sleep(0.1);
    recvMsg(cfd, &temp);
    string results = temp;
    if (results == "无") {
        cout << "无" << endl;
        return 1;
    }
    vector<string> list;
    group_analyse(results, list);
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    cout << "请选择你要进行的群id" << endl;
    string gid;
    while (1) {
        cin >> gid;
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == gid) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的群聊id不正确，请重新输入" << endl;
            continue;
        }
    }
    sendMsg(cfd, gid, strlen(gid.c_str()));
    sleep(0.5);
    recvMsg(cfd, &temp);
    if (strcmp(temp , "你具备权限") == 0) {
        sleep(0.1);
        recvMsg(cfd, &temp);
        printf("%s\n", temp);
        cout << "请在群成员中选择一位你要删除的群管理员" << endl;
        string id;
        cin >> id;
        sendMsg(cfd, id, strlen(id.c_str()));
        sleep(0.5);
        recvMsg(cfd, &temp);
        if (strcmp(temp, "yes") == 0) {
            printf("恭喜你删除成功\n");
        } else {
            printf("删除失败,不存在此人\n");
        }
    } else {//不具备权限
        printf("%s\n", temp);
        cout << "请重新进入此页面进行选择" << endl;
    }
    free(temp);
    return 1;
}

//删除群成员
int delete_member(int cfd) {
    string now = "Deltemember";
    sendMsg(cfd, now, strlen(now.c_str()));
    char *temp;
    sleep(0.1);
    recvMsg(cfd, &temp);
    string results = temp;
    if (results == "无") {
        cout << "无" << endl;
        return 1;
    }
    vector<string> list;
    group_analyse(results, list);
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    cout << "请选择你要进行的群id" << endl;
    string gid;
    while (1) {
        cin >> gid;
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == gid) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的群聊id不正确，请重新输入" << endl;
            continue;
        }
    }
    sendMsg(cfd, gid, strlen(gid.c_str()));
    sleep(0.1);
    recvMsg(cfd, &temp);
    sleep(0.1);
    if (strcmp(temp , "你具备权限") == 0) {
        sleep(0.1);
        recvMsg(cfd, &temp);
        string now = temp;
        printf("%s\n", temp);
        cout << "请在以上群成员中选择一位你要删除的群成员" << endl;
        string id;
        cin >> id;
        sendMsg(cfd, id, strlen(id.c_str()));
        sleep(0.5);
        recvMsg(cfd, &temp);
        if (strcmp(temp, "yes") == 0) {
            printf("恭喜你删除成功\n");
        } else {
            printf("删除失败,不存在此人\n");
        }
    } else {//不具备权限
        printf("%s\n", temp);
        cout << "请重新进入此页面进行选择" << endl;
    }
    free(temp);
    return 1;
}

//群聊天界面
int group_chatmenu(int cfd) {
    string choice;
    while (1) {
        cout << "       0.返回上一层    " << endl;
        cout << "       1.聊天         " << endl;
        cout << "       2.发送文件      " << endl;
        cout << "       3.接收文件      " << endl;
        cin >> choice;
        if (choice == "1") {
            int n = group_chat(cfd);
        } else if (choice == "2") {
            int n = group_sendfile(cfd);
        } else if (choice == "3") {
            int n = group_recvfile(cfd);
        } else if (choice == "0"){
            break; 
        } else {
            continue;
        }
    }
    return 1;
}

//群发送文件
int group_sendfile(int cfd) {
    string a1 = "SendfileGroup";
    sendMsg(cfd, a1, strlen(a1.c_str()));
    info file;
    int size = 0;
    int sum = 0;
    struct stat sta;//文件属性信息
    sleep(0.1);
    char *b;
    recvMsg(cfd, &b);
    string buff = b;//记录着自己的所有组别
    free(b);
    cout << "请从下面组别中选择一名" << endl;
    cout << buff << endl;
    string s;
    cout << "请输入发送对象的id" << endl;
    cin >> s;
    sendMsg(cfd, s, strlen(s.c_str()));

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

//群接收文件
int group_recvfile(int cfd) {
    recvfile(cfd);
    return 1;
}


//群聊天
int group_chat(int cfd) {

    string now = "Chatgroup";
    sendMsg(cfd, now, strlen(now.c_str()));
    sleep(0.1);
    char *temp;
    recvMsg(cfd, &temp);
    string buff = temp;
    cout << "请选择你要进行群聊的id" << endl;
    vector<string> list;
    group_analyse(buff, list);
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    string gid;
    cout << "请输入群聊id" << endl;
    while (1) {
        cin >> gid;
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == gid) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的群聊id不正确，请重新输入" << endl;
            continue;
        }
    }
    sendMsg(cfd, gid, strlen(gid.c_str()));
    cout << "现在请开始聊天吧" << endl;
    //下面直接开始聊天
        pthread_t tid;
        pthread_create(&tid, NULL, Seng, &cfd);//发送消息
        pthread_t tid1;
        pthread_create(&tid1, NULL, Reg, &cfd);//接收消息
        //这里千万不敢用detach，吓死
        pthread_join(tid, NULL);
        pthread_join(tid1, NULL);
    return 1;
}

//不停地发送消息
void *Seng(void* argc) {
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
void *Reg(void* argc) {//不停地接收消息
    int *n = (int *)argc;
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = *n;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, *n, &ev);
    struct epoll_event evs[1];
    char *buf;
    string r;
    while (1) {
        epoll_wait(epfd, evs, 1, -1);
        recvMsg(*n, &buf);
        if (strcmp("exit", buf) == 0) {//自己退出程序的具体过程
            break;
        }
        r = buf;
        cout << r << endl;
    }
    free(buf);
    return NULL;
}


//群聊消息的查看
int group_history(int cfd) {
    string now = "Historygroup";
    sendMsg(cfd, now, strlen(now.c_str()));
    sleep(0.1);
    char *temp;
    recvMsg(cfd, &temp);
    string buff = temp;
    cout << "请选择你要进行群聊的id" << endl;
    vector<string> list;
    group_analyse(buff, list);
    for (int i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
    string gid;
    cout << "请输入群聊id" << endl;
    while (1) {
        cin >> gid;
        int flag = 0;
        for(auto i = list.begin(); i != list.end(); i++) {
            if (*i == gid) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        } else {
            cout << "你输入的群聊id不正确，请重新输入" << endl;
            continue;
        }
    }
    sendMsg(cfd, gid, strlen(gid.c_str()));
    sleep(0.1);
    recvMsg(cfd, &temp);
    printf("%s\n", temp);
    return 1;
}