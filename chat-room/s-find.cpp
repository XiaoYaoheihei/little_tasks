//服务器查看消息管理
#include "epoll.h"
#include "funcs.h"
using namespace std;
extern std::multimap<std::string, std::string> inf;
extern std::multimap<std::string, std::string> groupinf;
//查看好友请求添加的消息
int findinfo(int cfd, string& info1) {
    //第二个参数便是自身的id信息
    string useid = info1;
    //在容器中查找useid
   
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    int i = 0;
    for(auto first = inf.lower_bound(useid),
        end = inf.upper_bound(useid); first != end;
        ++first) {
            i++;
            char *b;
            sendMsg(cfd, first->second, strlen((first->second).c_str()));
            epoll_wait(epfd, evs, 1, -1);
            recvMsg(cfd, &b);
            string now = b;
            if (now == "YES") {
                //在第一个数据库中添加此好友
                //并且在第二个数据库中添加新的id
                string& s2 = first->second;
                int n = add_base(useid, first->second);
                
                if (n == 1) {
                    cout << "add success" << endl;
                } else {
                    cout << "add fail" << endl;
                }
            } else if (now == "NO"){//不同意
                continue;
                // inf.erase(useid);//直接删除这个请求 
            }
            free(b);
        }
    auto n = inf.erase(useid);
    cout << "n = " << n << "\n" << "i =" << i << endl;
    string now = "END";
    sendMsg(cfd, now, strlen(now.c_str()));
    return 1;

}
//在数据库中添加id信息
int add_base(string& s1, string& s2) {
    int flag = 0;
    leveldb::Iterator* ita = db->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key(); 
        if (key.ToString() == s1) {
            leveldb::Slice s3 = ita->value();//首先转化成string类型
            string s5 = s3.ToString();
            auto s4 = nlohmann::ordered_json::parse(s5);
            s4["friends"] += s2;
            auto s = to_string(s4);
            db->Delete(leveldb::WriteOptions(), key);
            db->Put(leveldb::WriteOptions(), key, s);
            flag++;
            break;
        }   
        ita->Next();
    }
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key(); 
        if (key.ToString() == s2) {
            leveldb::Slice s3 = ita->value();//首先转化成string类型
            string s5 = s3.ToString();
            auto s4 = nlohmann::ordered_json::parse(s5);
            s4["friends"] += s1;
            auto s = to_string(s4);
            db->Delete(leveldb::WriteOptions(), key);
            db->Put(leveldb::WriteOptions(), key, s);
            flag++;
            break;
        }   
        ita->Next();
    }
    delete ita;
    //建立第二个数据库存储历史记录
    string target = s1 + "-" + s2;
    auto history = R"(
        {
            "history":[
                "现在请开始你们的聊天吧"
            ]
        }
        )";
    auto jn = nlohmann::ordered_json:: parse(history); 
    auto jn_1 = to_string(jn);
    leveldb::Status status1 = db1->Put(leveldb::WriteOptions(), target, jn_1);
    if (status1.ok()) {
        std::cout << status1.ToString() << std::endl;
    }
    cout << flag << endl;
    if (flag == 2) {
        return 1;
    } else {
        return -1;
    }
}

//查看群聊请求添加消息
int findgroup(int cfd, string& useid) {
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    if (groupinf.size() == 0) {
        string now = "END";
        sendMsg(cfd, now, strlen(now.c_str()));
        return 1;
    }
    leveldb::Iterator *ita = db2->NewIterator(leveldb::ReadOptions());
    int flag = 0;
        ita->SeekToFirst();
        string groupid;
        while (ita->Valid()) {
            leveldb::Slice key = ita->key();
            auto gid = key.ToString();
            //如果群消息容器里面有数据库中的群的话
            //判断群主的匹配情况
                leveldb::Slice s1 = ita->value();
                string s2 = s1.ToString();
                auto s3 = nlohmann::json::parse(s2);
                auto s4 = s3["群主"].get<string>();
                if (s4 == useid) {//群主匹配的话
                    groupid = gid;
                    cout << s4 << endl;
                    flag = 1;
                    break;
                }
                ita->Next();
        }
            if (flag == 1) {
                cout << "开始添加消息了" << endl;
                for(auto first = groupinf.lower_bound(groupid),
                end = groupinf.upper_bound(groupid); first != end;
                ++first) {
                    cout << first->second << endl;
                    sendMsg(cfd, first->second, strlen((first->second).c_str()));
                    char *b;
                    epoll_wait(epfd, evs, 1, -1);
                    recvMsg(cfd, &b);
                    string now = b;
                    if (now == "YES") {//同意的话
                        int n = add_to_group(groupid, first->second);
                        //groupinf.erase(groupid);
                        //多了这一步导致每一次同意成功之后都会引发段错误
                    } else if (now == "NO") {//不同意的话
                        continue;
                        //groupinf.erase(groupid);
                    }
                    free(b);
                }
                groupinf.erase(groupid);
            }
    string now = "END";
    sendMsg(cfd, now, strlen(now.c_str()));
    return 1;
}

//往群聊消息数据库当中添加消息
int add_to_group(string& groupid, string& id) {
    int flag = 0;
    leveldb::Iterator* ita = db2->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == groupid) {//检测到有此群,加到群里面去
            leveldb::Slice s1 = ita->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::json::parse(s2);
            s3.push_back(nlohmann::json::object_t::value_type(id, "成员"));
            s3["allmember"] += id;
            s3["expmember"] += id;
            auto s4 = to_string(s3);
            db2->Put(leveldb::WriteOptions(), key, s4);
            flag++;
            break;
        }
        ita->Next();
    }
    delete ita;
    if (flag == 1) {
        leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
        ita1->SeekToFirst();
        while (ita1-> Valid()) {
            leveldb::Slice key = ita1->key();
            if (key.ToString() == id) {//自己的群加进去
                leveldb::Slice s1 = ita1->value();
                string s2 = s1.ToString();
                auto s3 = nlohmann::ordered_json::parse(s2);
                s3["group"] += groupid;
                auto s4 = to_string(s3);
                db->Put(leveldb::WriteOptions(), key, s4);
                flag ++;
                break;
            }
            ita1->Next();
        }
        delete ita1;
    } 
    return 1;
}

//聊天数据库中查看历史记录
int history(int cfd, string& info) {
    //首先将好友信息发给客户端
    leveldb::Iterator *ita = db->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == info) {
            leveldb::Slice content = ita->value();
            auto jn =  content.ToString();
            sendMsg(cfd, jn, strlen(jn.c_str()));
            break;
        }
        ita->Next();
    }
    delete ita;
    char *buf;
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    int len = recvMsg(cfd, &buf);
    string s = buf;//存储着发送过来的k
    string a1, a2, temp1, temp2, his;
    free(buf);
    fenge(s, a1, a2);
    //cout << a1 << " " << a2 << endl;
    leveldb::Iterator *ita1 = db1->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    int flag = 0;
    while (ita1->Valid()) {
        leveldb::Slice key = ita1->key();
        auto now = key.ToString();
        fenge(now, temp1, temp2);
        //cout << temp1 << " " << temp2 << endl;
        if (((temp1 == a1)&&(temp2 == a2) )||((temp1 == a2)&&(temp2 == a1))) {
            leveldb::Slice value = ita1->value();
            //将含有josn格式的字符串进行提取
            auto now = value.ToString();
            auto jn = nlohmann::ordered_json::parse(now);
            vector<string> str;
            str = jn["history"].get<vector<string>>();
            string results;
            for(auto i : str) {
                results = results + i + "\n";
            }
            
            his = results;
            flag = 1; 
            break;
        }
        ita1->Next();
    }
    delete ita1;
    sendMsg(cfd, his, strlen(his.c_str()));
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
}