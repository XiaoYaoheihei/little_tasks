//服务器好友管理
#include "epoll.h"
#include "funcs.h"
using namespace std;
multimap<string, string> inf;
map<int ,string> flag_pos;
std::map<int , int > file_pos;
extern map<string, int> online;
extern std::map<int, int> inform;
//查看好友信息
int findfriends(int cfd, string& id) {
    leveldb::Iterator *ita = db->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    int flag = 0;
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == id) {
            leveldb::Slice content = ita->value();
            auto jn =  content.ToString();
            sendMsg(cfd, jn, strlen(jn.c_str()));
            flag = 1;
            break;
        }
        ita->Next();
    }
    if (flag == 1) {
        cout << "好友信息发送成功" << endl;
        return 1;
    } else {
        return -1;
    }
}

//删除好友,删除好友关系之后并没有把两者之间的聊天记录删除完
int blip(int cfd, string& info1, int m) {
    cout << "blip" << endl;
    string useid = info1;//记录登录人的id
    leveldb::Iterator* ita = db->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == useid) {
            leveldb::Slice content = ita->value();
            auto jn =  content.ToString();
            cout << jn << endl;
            int re = sendMsg(cfd, jn, strlen(jn.c_str()));
            cout << "re = " << re << endl;
            break;
        }
        ita->Next();
    }
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
    recvMsg(cfd, &buff);
    string use = buff;//记录需要删除的人的id
    free(buff);
    int flag = 0;
    
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == useid) {//是登录人的id
            leveldb::Slice s1 = ita->value();
            leveldb::Status ss = db->Delete(leveldb::WriteOptions(), key);
            if (ss.ok()) {
                cout << ss.ok() << endl;
                cout << key.ToString() << endl;;
            }
            string s2 = s1.ToString();
            auto s3 = nlohmann::json::parse(s2);
            std::vector<std::string> str ;
            str = s3["friends"].get<vector<string>>();
            s3.erase("friends");//将信息放到迭代器里面处理
            for (auto i = str.begin(); i != str.end();) {
                if (*i == use) {
                    i = str.erase(i);
                    continue;
                } 
                s3["friends"] += (*i);
                i++;
            }
            if (str.size() == 0) {
                std::vector<std::string> abc;
                s3.push_back(nlohmann::json::object_t::value_type("friends", abc));
            }
            auto s4 = to_string(s3);
            cout << s4 << endl;
            db->Put(leveldb::WriteOptions(), key, s4);
            flag++;
            break;
        }
        ita->Next();
    }
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == use) {//是要删除的人的id
            leveldb::Slice s1 = ita->value();
            leveldb::Status ss = db->Delete(leveldb::WriteOptions(), key);
            if (ss.ok()) {
                cout << ss.ok() << endl;
                cout << key.ToString() << endl;;
            }
            string s2 = s1.ToString();
            auto s3 = nlohmann::json::parse(s2);
            auto str = s3["friends"].get<vector<string>>();
            s3.erase("friends");//将信息放到迭代器里面处理
            for (auto it = str.begin(); it != str.end();) {
                if (*it == useid) {
                    it = str.erase(it);
                    continue;
                } 
                s3["friends"] += (*it);
                it++;
            }
            if (str.size() == 0) {
                std::vector<std::string> kong;
                s3.push_back(nlohmann::json::object_t::value_type("friends", kong));
            }
            auto s4 = to_string(s3);
            db->Put(leveldb::WriteOptions(), key, s4);
            flag++;
            break;
        }
        ita->Next();
    }
    if (flag == 2) {
        string now = "YES";
        sendMsg(cfd, now, 3);
        return 1;
    } else {
        string now = "NO";
        sendMsg(cfd, now, 3);
        return -1;
    }
    
}

//添加好友
int add(int cfd, string& info1, int number) {
    string useid = info1;//记录登录人的id
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
    recvMsg(cfd, &buff);
    string s = buff;//需要添加的人的id
    free(buff);
    if (online.count(s) == 1) {//如果用户处于在线状态
        auto iter = online.find(s);
        auto targetfd = iter->second;
        auto result = inform.find(targetfd);
        auto fd1 = result->second;
        string now = "您收到" + useid + "的请求好友添加消息";
        sendMsg(fd1, now, strlen(now.c_str()));
    }   
    inf.insert({s, useid});
    string now = "YES";
    sendMsg(cfd, now, strlen(now.c_str()));
    return 1;
}

int chat (int cfd, string& info) {//和好友进行聊天
    for (auto i = inform.begin(); i != inform.end(); i++) {
        std::cout << i->first << "   " << i->second << std::endl;
    }
    //首先将好友信息发给客户端
    leveldb::Iterator *ita = db->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == info) {
            leveldb::Slice content = ita->value();
            auto jn =  content.ToString();
            cout << jn << endl;
            int re = sendMsg(cfd, jn, strlen(jn.c_str()));
            cout << "re = " << re << endl;
            break;
        }
        ita->Next();
    }
    char *buff;
    int len, targetfd;
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //用来存放已经就绪的文件描述符的结构体数组
    epoll_wait(epfd, evs, 1, -1);
    len = recvMsg(cfd, &buff);
    string s = buff;//进行聊天的好友id
    int flag = 0;
    ita->SeekToFirst();
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == info) {
            leveldb::Slice content = ita->value();
            auto jn =  content.ToString();
            auto jn_1 = nlohmann::ordered_json::parse(jn);
            auto str = jn_1["sheild_f"].get<set<string>>();
            if (str.count(s) == 1) {//屏蔽的好友中找到了这个人
                string now = "好友已经屏蔽,无法聊天";
                sendMsg(cfd, now, strlen(now.c_str()));
                flag = 1;
            }
            break;
        }
        ita->Next();
    }
    delete ita;
    if (flag == 1) {
        return 1;
    } else {
        string now = "开始聊天吧";
        sendMsg(cfd, now, strlen(now.c_str()));
    }
    flag_pos.insert({cfd, s});  //1.全局标志此时的聊天对象
                                //2.证明自己进入了聊天界面
    string s1 = info;//记录自己的id
    string s3 = s1 + "-" + s;//第二个数据库的k
    free(buff);
    string chatid = s1;
            //1.不在线
            //2.在线，不处于聊天界面
            //3.在线，处于聊天界面，聊天人有题
            //4.在线，突然进入聊天界面
    char *tempt;
    
    while (1) {//应该先接收消息，再判断是否在线
        epoll_wait(epfd, evs, 1, -1);
        recvMsg(cfd, &tempt);
        if (online.count(s) == 0) {//不在线
            
            string s1 = tempt;
            if (s1 == "exit") {//收到的是退出的话
                                //不对信息进行存储
                //往好友信息map中设置位置
                //在服务器端的vector中添加元素，k-目标id，v-存在消息
                string content ="收到来自" + s + "的消息"; 
                inf.insert({s3, content});  
                sendMsg(cfd, s1, strlen(s1.c_str()));
                break;
            }
            string now = chatid + ":" + s1;
            int n = store(s3, now);
            
        } else {//在线
            auto iter = online.find(s);
            targetfd = iter->second;
            if (flag_pos.count(targetfd) == 1) {//处于聊天界面
                string s1 = tempt;
                if (s1 == "exit") {//不进行历史消息的存储
                    flag_pos.erase(cfd);//取消掉标志位
                    sendMsg(cfd, s1, strlen(s1.c_str()));
                    break;
                }
                string now = "      " + chatid + ":" + s1;
                int n = store(s3, now);
                auto ss1 = flag_pos.find(targetfd);//开始判断聊天对象
                auto ss2 = ss1->second;
                if  (ss2 == chatid) {//如果别人的聊天对象刚好是自己的话
                    sendMsg(targetfd, now, strlen(now.c_str()));
                } else {
                    auto result = inform.find(targetfd);
                    auto fd1 = result->second;
                    now = "您收到" + chatid + "发来的聊天消息";
                    cout << fd1 << endl;
                    cout << now << endl;
                    sendMsg(fd1, now, strlen(now.c_str()));
                }
            } else {//没有处于聊天界面
                string s1 = tempt;
                //往好友信息map中设置位置
                //在服务器端的vector中添加元素，k-目标id，v-存在消息
                if (s1 == "exit") {//当用户输入的是exit的时候，不进行存储
                    sendMsg(cfd, s1, strlen(s1.c_str()));
                    break;
                }
                string now =  chatid + ":" + s1;
                int n = store(s3, now);
                auto result = inform.find(targetfd);
                auto fd1 = result->second;
                now = "您收到" + chatid + "发来的聊天消息";
                cout << fd1 << endl;
                cout << now << endl;
                sendMsg(fd1, now, strlen(now.c_str()));
            }
            }    
        }
        free(tempt);
        return 1;
    }
   

//服务器进行历史消息的存储
int store(string s, string ss) {//参数分别是id和消息
    string s1, s2;//记录着分割的id信息
    fenge(s, s1, s2);//首先对id进行分割判断
    string temp1, temp2;
    leveldb::Iterator *ita = db1->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    int flag = 0;
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        auto now = key.ToString();
        fenge(now, temp1, temp2);
        if (((temp1 == s1)&&(temp2 == s2))||((temp1 == s2)&&(temp2 == s1))) {
            //把内容拿出来转成josn格式才可以存储
            leveldb::Slice value = ita->value();
            auto s = value.ToString();
            auto jn = nlohmann::ordered_json::parse(s);
            jn["history"] += ss;
            auto jn_1 = to_string(jn);
            leveldb::Status status = db1->Put(leveldb::WriteOptions(), key, jn_1);
            if (status.ok()) {
                cout << status.ok() << endl;
                flag = 1;
            } 
            break;
        }
        ita->Next();
    }
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
}
//对聊天的key进行分解
void fenge(string s, string& temp1, string& temp2) {
    stringstream ss(s);
    char c = '-';
    vector<string> results;
    string str;
    while (getline(ss, str, c)) {
        results.push_back(str);
    }
    temp1 = results[0];
    temp2 = results[1];
}
//文件主界面
int filemenu(int cfd, string& useid) {
    leveldb::Iterator *ita = db->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == useid) {
            leveldb::Slice content = ita->value();
            auto jn =  content.ToString();
            cout << jn << endl;
            sendMsg(cfd, jn, strlen(jn.c_str()));
            break;
        }
        ita->Next();
    }
    delete ita;
    char *buff;
    int len, targetfd;
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //用来存放已经就绪的文件描述符的结构体数组
    epoll_wait(epfd, evs, 1, -1);
    len = recvMsg(cfd, &buff);
    string s = buff;//好友id
    free(buff);
    string path;
    info *file;
    int n = recvfile(cfd, useid, path, s);
    // int m = sendfile(s, useid, path, file);
    return 1;
}
//接收文件
int recvfile (int cfd, string& useid, string& path, string posid){
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //服务器端数据库开始接收文件
    info file;
    long long size = 0;
    long long recvsize = 0;
    epoll_wait(epfd, evs, 1, -1);
    int len = read(cfd, &file, sizeof(struct file_information));
    if (len <= 0) {
        cout << "接收文件基本信息失败" << endl;
        perror("read");
        return -1;
    }
    auto filesize = file.file_length;
    cout << filesize << endl;
    string filename = file.file_name;
    string filepath = "/home/xiaoyao/database1/" + useid + "_" + filename;
    //服务器本地新建文件地址
    int filefd = open(filepath.c_str(),  O_RDWR | O_CREAT | O_APPEND, 0644);
    //特别注意权限问题
    if (filefd == -1) {
        perror("open");
        return -1;
    }
    char buffer[1024];
    while (1) {
        //sleep(1);//直接从内核缓冲区里面读取信息
        size = read(cfd, buffer, sizeof(buffer));
        if (size == -1) {
            if (errno == EINTR || errno ==  EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
        }
        // cout << "size = " << size << endl;
        else if (size > 0) {
            recvsize += size;
            int ret = write(filefd, buffer, size);//文件里面书写
            // cout << "size = " << size << endl;
            memset(buffer, 0, sizeof(buffer));
            if (recvsize == filesize) { 
                break;
            }
        } 
    }
    cout << "开始向客户端发送文件" << endl;
    //开始向客户端发送文件
    if (online.count(posid) == 0) {
        return -1;
    } 
    auto now = online.find(posid);
    auto targetfd = now->second;//找到目标fd之后
    write(targetfd, &file, sizeof(struct file_information));
    long long size1 = 0;
    long long sendsize = 0;
    long long r = 0;
    int filefd1 = open(filepath.c_str(), O_RDWR | O_CREAT | O_APPEND, 0644);
    // string filecontent;
    while (1) {
        size1 = read(filefd1, buffer, sizeof(buffer));
        int re = write(targetfd, buffer, size1);
        if (re > 0) {
            sendsize += re;
            memset(buffer, 0, sizeof(buffer));
        }
        if (size1 > re) {
            lseek(filefd1, sendsize, SEEK_SET);
        }
        if (sendsize >= filesize) {
            break;
        }
    }
    // while (1) {
    //     //sleep(1);
    //     size1 = read(filefd1, buffer, sizeof(buffer));
    //     if(size1<0){
    //         throw std::runtime_error("Read failed\n");
    //     }
    //     r += size1;
    //     cout << "size1 = " << size1 << endl;
    //     int re = write(targetfd, buffer, size1);
    //     if (re != -1 && re == size1) {//发送的和接收的完全一致
    //         sendsize += re;
    //         //cout << re << endl;
    //         memset(buffer, 0, sizeof(buffer));
            
    //     } else if (ret == -1){//没有发送成功重新发送
    //         //sleep(0.5);
    //         continue;
    //     } else if (re != -1 && re != size1) {//发送的和接收的有一部分偏差
    //         lseek(filefd1, sendsize, SEEK_SET);
    //     }
    //     if (sendsize == filesize){
    //         cout << filesize << "\n" << r << "\n" << sendsize << endl;
    //         printf("发送完成\n");
    //         close(filefd1);
    //         break;
    //     }
    // }
    return 1;
 }
// //发送文件
// int sendfile(string posid, string& useid, string& path,struct file_information * file) {
//     if (online.count(posid) == 0) {
//         return -1;
//     } 
//     auto now = online.find(posid);
//     auto targetfd = now->second;//找到目标fd之后
//     string filepath = path;
//     write(targetfd, file, sizeof(struct file_information));
//     unsigned long int size ;
//     int filefd = open(filepath.c_str(),  O_WRONLY | O_CREAT | O_APPEND);
//     char buffer[1024];
//     while (1) {
//         size = read(filefd, buffer, sizeof(buffer));
//         if (size > 0) {

//             write(targetfd, buffer, size);
//             memset(buffer, 0, sizeof(buffer));
//         } else if (size == 0){
//             printf("发送完成");
//             close(filefd);
//             break;
//         }
//     }
//     return 1;
// }
//进行屏蔽好友
int sheild(int cfd, string& useid) {
    leveldb::Iterator *ita = db->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    int flag = 0;
    //首先将好友信息发给客户端
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == useid) {
            //把内容拿出来转成josn格式才可以存储
            leveldb::Slice value = ita->value();
            auto s = value.ToString();
            cout << s << endl;
            int re = sendMsg(cfd, s, strlen(s.c_str()));
            cout << "re = " << re << endl;
            break;
        }
        ita->Next();
    }
    
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //用来存放已经就绪的文件描述符的结构体数组
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    string posid = buff;//目标id
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == useid) {
            //把内容拿出来转成josn格式才可以存储
            leveldb::Slice value = ita->value();
            auto s = value.ToString();
            auto jn = nlohmann::ordered_json::parse(s);
            jn["sheild_f"] += posid;
            auto jn_1 = to_string(jn);
            db->Put(leveldb::WriteOptions(), key, jn_1);
            break;
        }
        ita->Next();
    }
    delete ita;
    string r = "屏蔽成功";
    sendMsg(cfd, r, strlen(r.c_str()));
    free(buff);
    return 1;
}

//查看好友在线信息
int f_online(int cfd, string& useid) {
    leveldb::Iterator *ita = db->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    string r;
     //首先将好友信息发给客户端
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == useid) {
            //把内容拿出来转成josn格式才可以存储
            leveldb::Slice value = ita->value();
            auto s = value.ToString();
            cout << s << endl;
            int re = sendMsg(cfd, s, strlen(s.c_str()));
            cout << "re = " << re << endl;
            break;
        }
        ita->Next();
    }
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    //用来存放已经就绪的文件描述符的结构体数组
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    string posid = buff;//目标id
    string re;
    int flag = 0;
    if (online.count(posid) == 1) {//如果找到在线信息
        re = "在线";
        sendMsg(cfd, re, strlen(re.c_str()));
        flag = 1;
    } else {
        re = "不在线";
        sendMsg(cfd, re, strlen(re.c_str()));

    }
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
}