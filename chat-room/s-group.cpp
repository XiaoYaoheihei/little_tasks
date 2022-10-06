//服务器群管理
#include "epoll.h"
#include "funcs.h"
using namespace std;
extern std::map<int, int> inform;
map<int , std::string> chat_pos;
std::multimap<std::string, std::string> groupinf;
//群组的创建
int gcreat(int cfd, string& useid) {
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    int flag = 0;
    string useid1 = buff;//传过来的群id
    int cout = 0;
    leveldb::Iterator* ita1 = db2->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    while (ita1->Valid()) {//判断以前是否创建过此id
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid1) {//传过来的id
            cout = 1;
            break;
        }
        ita1->Next();
    }
    if (cout == 1) {
        string a = "NO";
        sendMsg(cfd, a, strlen(a.c_str()));
        return -1;
    }
    delete ita1;
    leveldb::Iterator* ita = db->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == useid) {//是登录人的id
            leveldb::Slice s1 = ita->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            s3["group"] += useid1;
            auto s4 = to_string(s3);
            db->Put(leveldb::WriteOptions(), key, s4);
            flag++;
            break;
        }
        ita->Next();
    }
    delete ita;
    //在第三个数据库中进行消息存储
    string content;
    g_analys(useid, content);//对群进行初始化
    db2->Put(leveldb::WriteOptions(), useid1, content);
    flag++;
    string a;
    if (flag == 2) {
        a = "YES";
        sendMsg(cfd, a, strlen(a.c_str()));
        return 1;
    } else {
        a = "NO";
        sendMsg(cfd, a, strlen(a.c_str()));
        return -1;
    }
}

//群主创建群的初始化
int g_analys(string useid, string& s) {
    auto jn_1 = R"(
        {
            "history":["开始聊天吧"],
            "expmember":[],
            "allmember":[],
            "administrators":[]
        }
    )";
    auto jn_2 = nlohmann::json::parse(jn_1);
    jn_2["allmember"] += useid;
    jn_2.push_back(nlohmann::json::object_t::value_type("群主", useid));
    s = to_string(jn_2);
    return 1;
}

//群组的解散
int gdelete(int cfd, string& useid) {
    leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    string message;
    int flags = 0;
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            std::vector<std::string> str;
            str = s3["group"].get<std::vector<std::string>>();
            if (str.size() == 0) {//如果没有群的话直接退出
                message = "无";
                flags = 1;
                break;
            }
            message = s2;
            break;
        }
        ita1->Next();
    }
    sendMsg(cfd, message, strlen(message.c_str()));
    if (flags == 1) {
        return 1;
    }
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    string gid = buff;//记录了要解散的群id
    free(buff);
    leveldb::Iterator* ita = db2->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    vector<string> member;//记录着群聊的所有成员
    int flag = 0; 
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == gid) {//检测到此群
            leveldb::Slice s1 = ita->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            auto s4 = s3["群主"].get<string>();
            string condi;
            if (s4 == useid) {//开始解散群
                condi = "你具备权限";
                sendMsg(cfd, condi, strlen(condi.c_str()));
            } else {
                condi = "你不具备权限";
                sendMsg(cfd, condi, strlen(condi.c_str()));
                flag = 1;
                break;
            }
            member = s3["allmember"].get<vector<string>>();
            auto s = db2->Delete(leveldb::WriteOptions(), key);
            break;
        }
        ita->Next();
    }
    delete ita;
    if (flag == 1) {
        return 1;
    }
    //开始删除每一个成员的相关group
    //这里怎么设计了一个三重循环，不好的设计
    for (auto i = member.begin(); i != member.end(); i++) {
        ita1->SeekToFirst();
        while (ita1-> Valid()) {
            leveldb::Slice key = ita1->key();
            cout << "key == " << key.ToString() << endl;
            if (key.ToString() == *i) {
                leveldb::Slice s1 = ita1->value();
                string s2 = s1.ToString();
                auto s3 = nlohmann::json::parse(s2);
                std::vector<std::string> str;
                str = s3["group"].get<std::vector<std::string>>();
                s3.erase("group");
                for (auto j = str.begin(); j != str.end();) {
                    if (*j == gid) {
                        j = str.erase(j);
                        continue;
                    }
                    j++;
                }
                s3.push_back(nlohmann::json::object_t::value_type("group", str));
                auto s4 = to_string(s3);
                db->Put(leveldb::WriteOptions(), key, s4);
                break;
            }
            ita1->Next();
        }
    }
    cout << "所有成员组删除完成" << endl;
    delete ita1;
    return 1;
}

//申请加入群聊
int gadd(int cfd, string& useid) {
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    string re = buff;//记录的是群的id
    free(buff);
    groupinf.insert({re, useid});//将群id和请求添加的id存到map中去
    //检查群主是否在线并且发送实时消息来请求添加
    string now = "NO";
    leveldb::Iterator* ita = db2->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == re) {//检测到有此群,加到群里面去
            leveldb::Slice s1 = ita->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::json::parse(s2);
            auto s4 = s3["群主"].get<string>();
            now = "YES";
            if (online.count(s4) == 1) {//群主在线的话
                auto s = online.find(s4);
                auto fd1 = s->second;
                auto now = inform.find(fd1);
                auto targetfd = now->second;
                string content = "你接收到来自" + useid + "的请求添加消息";
                sendMsg(targetfd, content, strlen(content.c_str()));
            }
            break;
        }
        ita->Next();
    }
    delete ita;
    sendMsg(cfd, now, strlen(now.c_str()));
    return 1;
}

//申请退群
int gquit(int cfd, string& useid) {
    string message;
    int flag = 0;
    leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            std::vector<std::string> str;
            str = s3["group"].get<std::vector<std::string>>();
            for (auto i : str) {
                message = message + i +"\n";
            }
            flag = 1;
            break;
        }
        ita1->Next();
    }
    
    if (flag == 0) {
        message = "无";
        sendMsg(cfd, message, strlen(message.c_str()));
        return 1;
    }
    sendMsg(cfd, message, strlen(message.c_str()));
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    string gid = buff;//需要退出的群
    free(buff);
    cout << "okok" << endl;
    int flag1 = 0;

    leveldb::Iterator* ita = db2->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        cout << "okok" << endl;
        if (key.ToString() == gid) {//检测到此群
            cout << "检测到了" << endl;
            leveldb::Slice s1 = ita->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::json::parse(s2);
            s3.erase(useid);
            vector<string> str;//成员容器
            str = s3["allmember"].get<vector<string>>();
            //所有成员的那个容器中删除掉相关内容
            s3.erase("allmember");
            for (auto i = str.begin(); i != str.end();) {
                if (*i == useid) {
                    i = str.erase(i);
                    continue;
                }
                i++;
            }
            s3.push_back(nlohmann::json::object_t::value_type("allmember", str));
            //管理员的那个容器中删除
            str = s3["administrators"].get<vector<string>>();
            s3.erase("administrators");
            for (auto i = str.begin(); i != str.end();) {
                if (*i == useid) {
                    i = str.erase(i);
                    continue;
                }
                i++;
            }
            s3.push_back(nlohmann::json::object_t::value_type("administrators", str));
            //除过管理员以外的容器删除
            str = s3["expmember"].get<vector<string>>();
            s3.erase("expmember");
            for (auto i = str.begin(); i != str.end();) {
                if (*i == useid) {
                    i = str.erase(i);
                    continue;
                }
                i++;
            }
            s3.push_back(nlohmann::json::object_t::value_type("expmember", str));
            auto s4 = to_string(s3);
            db2->Put(leveldb::WriteOptions(), key, s4);
            flag1++;
            break;
        }
        ita->Next();
    }
    delete ita;
    
    ita1->SeekToFirst();
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {//检测到此人
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::json::parse(s2);
            //删除掉此人组中的群id
            std::vector<std::string> str;
            str = s3["group"].get<vector<string>>();
            s3.erase("group");
            for (auto i = str.begin(); i != str.end();) {
                if (*i == gid) {
                    i = str.erase(i);
                    continue;
                }
                i++;
            }
            s3.push_back(nlohmann::json::object_t::value_type("group", str));
            auto s4 = to_string(s3);
            db->Put(leveldb::WriteOptions(), key, s4);
            flag1++;
            break;
        }
        ita1->Next();
    }
    delete ita1;
    if (flag1 == 2) {
        message = "退出成功";
        sendMsg(cfd, message, strlen(message.c_str()));
    } else {
        message = "退出失败";
        sendMsg(cfd, message, strlen(message.c_str()));
    }
    return 1;
}


//查看已经加入的群聊
int gfind(int cfd, string& useid) {
    string message;
    int flag = 0;
    leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            std::vector<std::string> str;
            str = s3["group"].get<std::vector<std::string>>();
            if (str.size() == 0) {//如果没有群的话直接退出
                break;
            }
            for (auto i : str) {
                message = message + i +"\n";
            }
            flag = 1;
            break;
        }
        ita1->Next();
    }
    delete ita1;
    if (flag == 0) {
        message = "无";
    }
    sendMsg(cfd, message, strlen(message.c_str()));
    return 1;
}

//查看群成员
int gmember(int cfd, string& useid) {
    string message;
    int flag = 0;

    leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            std::vector<std::string> str;
            str = s3["group"].get<std::vector<std::string>>();
            if (str.size() == 0) {
                break;
            }
            message = s2;
            flag++;
            break;
        }
        ita1->Next();
    }
    delete ita1;
    if (flag == 0) {
        message = "无";
    }
    sendMsg(cfd, message, strlen(message.c_str()));
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    string gid = buff;
    free(buff);
    string results;
    leveldb::Iterator* ita = db2->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        cout << key.ToString() << endl;
        if (key.ToString() == gid) {//检测到此群
            leveldb::Slice s1 = ita->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            std::vector<std::string> s;
            s = s3["allmember"].get<std::vector<std::string>>();
            for (auto k : s) {
                results = results + k +"\n";
            }
            flag++;
            break;
        }
        ita->Next();
    }
    delete ita;
    if (flag == 2) {
        sendMsg(cfd, results, strlen(results.c_str()));
    }
    return 1;
}

//添加管理员
int admiadd(int cfd, string& useid) {
    string message;
    int flag = 0;
    leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            std::vector<std::string> str;
            str = s3["group"].get<std::vector<std::string>>();
            if (str.size() == 0) {
                break;
            }
            message = s2;
            flag = 1;
            break;
        }
        ita1->Next();
    }
    delete ita1;
    if (flag == 0) {
        message = "无";
        sendMsg(cfd, message, strlen(message.c_str()));
        return 1;
    }
    sendMsg(cfd, message, strlen(message.c_str()));
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    string gid = buff;
    string condi;
    //先检查此人是否具有权限
    leveldb::Iterator* ita = db2->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == gid) {//检测到此群
            leveldb::Slice s1 = ita->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::json::parse(s2);
            auto s4 = s3["群主"].get<string>();
            if (s4 == useid) {
                condi = "你具备权限";
                sendMsg(cfd, condi, strlen(condi.c_str()));
            } else {
                condi = "你不具备权限";
                sendMsg(cfd, condi, strlen(condi.c_str()));
                break;
            }
            // cout << condi << endl;
            string r;
            vector<string> str;//除了管理人和群主之外的成员
            str = s3["expmember"].get<vector<string>>();
            for (auto i : str) {
                r = r + i + "\n";
            }
            cout << r.size() << endl;
            sendMsg(cfd, r, strlen(r.c_str()));
            // cout << "这里" << endl;
            epoll_wait(epfd, evs, 1, -1);
            recvMsg(cfd, &buff);//等待输入的人
            string member = buff;
            
            int flags = 2;//默认是不存在此人
            for (auto i = str.begin(); i != str.end(); i++) {
                if (*i == member) {
                    flags = 1;
                    break;
                } else {
                    flags = 0;
                }
            }
            if (flags == 0) {
                condi = "no";
                sendMsg(cfd, condi, strlen(condi.c_str()));
                return 1;
            }

            s3.erase(member);
            s3.push_back(nlohmann::json::object_t::value_type(member, "管理员"));
            s3["administrators"] += member;
            // str = s3["expmember"].get<vector<string>>();
            s3.erase("expmember");
            for (auto i = str.begin(); i != str.end(); ) {
                if (*i == member) {
                    i = str.erase(i);
                    continue;
                }
                i++;
            }
            
            s3.push_back(nlohmann::json::object_t::value_type("expmember", str));
            auto s5 = to_string(s3);
            db2->Put(leveldb::WriteOptions(), key, s5);
            condi = "yes";
            break;
        }
        ita->Next();
    }
    delete ita;
    sendMsg(cfd, condi, strlen(condi.c_str()));
    return 1;
    
}

//删除管理员
int admidelete(int cfd, string& useid) {
    string message;
    int flag = 0;
    leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            std::vector<std::string> str;
            str = s3["group"].get<std::vector<std::string>>();
            if (str.size() == 0) {
                break;
            }
            message = s2;
            flag = 1;
            break;
        }
        ita1->Next();
    }
    delete ita1;
    if (flag == 0) {
        message = "无";
        sendMsg(cfd, message, strlen(message.c_str()));
        return 1;
    }
    sendMsg(cfd, message, strlen(message.c_str()));
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    string gid = buff;
    string condi;
    //先检查此人是否具有权限
    leveldb::Iterator* ita = db2->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == gid) {//检测到此群
            leveldb::Slice s1 = ita->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::json::parse(s2);
            auto s4 = s3["群主"].get<string>();
            if (s4 == useid) {
                condi = "你具备权限";
                sendMsg(cfd, condi, strlen(condi.c_str()));
            } else {
                condi = "你不具备权限";
                sendMsg(cfd, condi, strlen(condi.c_str()));
                break;
            }
            string r;
            vector<string> str;//管理员容器
            str = s3["administrators"].get<vector<string>>();
            for (auto i : str) {
                r = r + i + "\n";
            }
            
            sendMsg(cfd, r, strlen(r.c_str()));
            //cout << r << endl;
            epoll_wait(epfd, evs, 1, -1);
            recvMsg(cfd, &buff);//等待输入的人
            string member = buff;
            
            int flags = 2;
            for (auto i = str.begin(); i != str.end(); i++) {
                if (*i == member) {
                    flags = 1;
                    break;
                } else {
                    flags = 0;
                }
            }
            if (flags == 0) {
                condi = "no";
                sendMsg(cfd, condi, strlen(condi.c_str()));
                return 1;
            }

            s3.erase(member);
            s3.erase("administrators");
            s3.push_back(nlohmann::json::object_t::value_type(member, "成员"));
            for (auto i = str.begin(); i != str.end();) {
                if (*i == member) {
                    i = str.erase(i);
                    continue;
                }
                i++;
            }
            s3.push_back(nlohmann::json::object_t::value_type("administrators", str));
            s3["expmember"] += member;
            auto s5 = to_string(s3);
            db2->Put(leveldb::WriteOptions(), key, s5);
            condi = "yes";
            break;
        }
        ita->Next();
    }
    delete ita;
    sendMsg(cfd, condi, strlen(condi.c_str()));
    return 1;
}

//删除群成员
int memberdelete(int cfd, string& useid) {
    string message;
    int flag = 0;
    leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            std::vector<std::string> str;
            str = s3["group"].get<std::vector<std::string>>();
            if (str.size() == 0) {
                break;
            }
            message = s2;
            flag = 1;
            break;
        }
        ita1->Next();
    }
    if (flag == 0) {
        message = "无";
        sendMsg(cfd, message, strlen(message.c_str()));
        return 1;
    }
    sendMsg(cfd, message, strlen(message.c_str()));
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    string gid = buff;
    string condi;
    string member;
    //先检查此人是否具有权限
    leveldb::Iterator* ita = db2->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == gid) {//检测到此群
            leveldb::Slice s1 = ita->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::json::parse(s2);
            auto s4 = s3["群主"].get<string>();
            string s5;
            if (s4 != useid) {
                s5 = s3[useid].get<string>();
            }
            if (s4 == useid || s5 == "管理员") {
                condi = "你具备权限";
                sendMsg(cfd, condi, strlen(condi.c_str()));
            } else {
                condi = "你不具备权限";
                sendMsg(cfd, condi, strlen(condi.c_str()));
                break;
            }
            string r;
            vector<string> str;//成员容器
            str = s3["expmember"].get<vector<string>>();
            for (auto i : str) {
                r = r + i + "\n";
            }
            sendMsg(cfd, r, strlen(r.c_str()));
            epoll_wait(epfd, evs, 1, -1);
            recvMsg(cfd, &buff);//等待输入的人
            member = buff;
            int flags = 0;//默认是不存在此人
            for (auto i = str.begin(); i != str.end(); i++) {
                if (*i == member) {
                    flags = 1;
                    break;
                }
            }
            if (flags == 0) {
                condi = "no";
                sendMsg(cfd, condi, strlen(condi.c_str()));
                return 1;
            }
            // auto s6 = s3[member].get<string>();
            // if (s6 == "管理员" || s6 == s4 || member == useid) {
            //     condi = "no";
            //     sendMsg(cfd, condi, strlen(condi.c_str()));
            //     break;
            // }
            s3.erase(member);
            //所有成员的那个容器中删除掉
            str = s3["allmember"].get<vector<string>>();
            s3.erase("allmember");
            for (auto i = str.begin(); i != str.end();) {
                if (*i == member) {
                    i = str.erase(i);
                    continue;
                }
                i++;
            }
            s3.push_back(nlohmann::json::object_t::value_type("allmember", str));
            //管理员的那个容器中删除
            str = s3["administrators"].get<vector<string>>();
            s3.erase("administrators");
            for (auto i = str.begin(); i != str.end();) {
                if (*i == member) {
                    i = str.erase(i);
                    continue;
                }
                i++;
            }
            s3.push_back(nlohmann::json::object_t::value_type("administrators", str));
            //除过管理员以外的容器删除
            str = s3["expmember"].get<vector<string>>();
            s3.erase("expmember");
            for (auto i = str.begin(); i != str.end();) {
                if (*i == member) {
                    i = str.erase(i);
                    continue;
                }
                i++;
            }
            s3.push_back(nlohmann::json::object_t::value_type("expmember", str));
            auto s7 = to_string(s3);
            db2->Put(leveldb::WriteOptions(), key, s7);
            flag++;
            break;
        }
    ita->Next();
    }
    if (flag == 2) {
        ita1->SeekToFirst();
        while (ita1-> Valid()) {
            leveldb::Slice key = ita1->key();
            if (key.ToString() == member) {
                leveldb::Slice s1 = ita1->value();
                string s2 = s1.ToString();
                auto s3 = nlohmann::json::parse(s2);
                std::vector<std::string> str;
                str = s3["group"].get<std::vector<std::string>>();
                s3.erase("group");
                for (auto i = str.begin(); i != str.end();) {
                    if (*i == gid) {
                        i = str.erase(i);
                        continue;
                    }
                    i++;
                }
                s3.push_back(nlohmann::json::object_t::value_type("group", str));
                auto s4 = to_string(s3);
                db->Put(leveldb::WriteOptions(), key, s4);
                flag++;
                break;
            }
            ita1->Next();
        }
    }
    if (flag == 3) {
        condi = "yes";
    } else {
        condi = "no";
    }
    sendMsg(cfd, condi, strlen(condi.c_str()));
    delete ita;
    delete ita1;
    return 1;
}

//群聊天
int groupchat(int cfd, string& useid) {
    string message;
    int flag = 0;
    leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            flag = 1;
            // auto s3 = nlohmann::ordered_json::parse(s2);
            // std::vector<std::string> str;
            // str = s3["group"].get<std::vector<std::string>>();
            // for (auto i : str) {
            //     message = message + i +"\n";
            // }
            message = s2;

            break;
        }
        ita1->Next();
    }
    delete ita1;
    if (flag == 0) {
        message = "无";
        sendMsg(cfd, message, strlen(message.c_str()));
        return 1;
    }
    sendMsg(cfd, message, strlen(message.c_str()));
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    char *buff;//群id
    recvMsg(cfd, &buff);
    printf("%s\n", buff);//打印接收到的群id
    string groupid = buff;
    free(buff);

    chat_pos.insert({cfd, groupid});//1.全局标志此时的聊天对象
                                    //2.证明自己进入了聊天界面
    char *tempt;
    string now;
            //1.不在线
            //2.在线，不处于聊天界面
            //3.在线，处于聊天界面，聊天人有题
            //4.在线，突然进入聊天界面
    while (1) {
        leveldb::Iterator* ita = db2->NewIterator(leveldb::ReadOptions());
        ita->SeekToFirst();
        int flag = 0;
        vector<string> member;//记录所有的成员
        while (ita-> Valid()) {
            leveldb::Slice key = ita->key();
            if (key.ToString() == groupid) {//检测到此群
                leveldb::Slice s1 = ita->value();
                string s2 = s1.ToString();
                auto s3 = nlohmann::json::parse(s2);
                member = s3["allmember"].get<vector<string>>();
                break;
            }
            ita->Next();
        }
        epoll_wait(epfd, evs, 1, -1);
        recvMsg(cfd, &tempt);//接收消息
        now = tempt;
        if (now == "exit") {
            chat_pos.erase(cfd);//取消掉自己的标志位
            sendMsg(cfd, now, strlen(now.c_str()));
            break;
        }
        int target;
        //开始判断群里面的每个成员是否都在线
        for (auto i = member.begin(); i != member.end(); i++) {
            if (*i == useid) {//成员如果是自己的话跳过
                continue;
            }
            if (online.count(*i) == 0) {//不在线
                string message = useid + ":" + now;
                int n = storeinfo(groupid, message);
            } else {//在线
                auto iter = online.find(*i);
                target = iter->second;//通信描述符
                if (chat_pos.count(target) == 1) {//处于群聊界面
                    string message = "    " + useid + ":" +now;
                    int n = storeinfo(groupid, message);
                    auto ss1 = chat_pos.find(target);//判断群聊对象
                    auto ss2 = ss1->second;
                    if (ss2 == groupid) {//聊天的群如果一样的话
                        sendMsg(target, message, strlen(message.c_str()));
                    } else {
                        auto result = inform.find(target);
                        auto fd1 = result->second;
                        message = "您收到" + useid + "发来的群聊消息";
                        sendMsg(fd1, message, strlen(message.c_str()));
                    }
                } else {//没有处于群聊界面 
                    string message = "    " + useid + ":" + now;
                    int n = storeinfo(groupid, message);
                    auto result = inform.find(target);
                    auto fd1 = result->second;
                    message = "您收到" + useid + "发来的群聊消息";
                    sendMsg(fd1, message, strlen(message.c_str()));
                }
            }
        }
        
    }
    free(tempt);
    return 1;
}

//历史消息记录的存储
int storeinfo(string& groupid, string& now) {
    leveldb::Iterator* ita = db2->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    while (ita-> Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == groupid) {//检测到此群
            leveldb::Slice s1 = ita->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::json::parse(s2);
            s3["history"] += now;
            auto s4 = to_string(s3);
            db2->Put(leveldb::WriteOptions(), key, s4);
            break;
        }
        ita->Next();
    }
    return 1;
}

//群文件相关的
int group_file(int cfd, string& useid) {
    int flag = 0;
    string message;
    leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            std::vector<std::string> str;
            str = s3["group"].get<std::vector<std::string>>();
            for (auto i : str) {
                message = message + i +"\n";
            }
            flag = 1;
            break;
        }
        ita1->Next();
    }
    delete ita1;
    if (flag == 0) {
        message = "无";
        sendMsg(cfd, message, strlen(message.c_str()));
    }
    //发送组别相关信息
    sendMsg(cfd, message, strlen(message.c_str()));
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
    string s = buff;//组别id
    free(buff);
    string path;
    info *file;
    int n = recvfile_group(cfd, useid, path, s);
    return 1;
}   

//接收文件
int recvfile_group(int cfd, string& useid, string& path, string groupid){
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
    string filepath = "/home/xiaoyao/database2/" + useid + "_" + filename;
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
        cout << size << endl;
        if (size > 0) {
            recvsize += size;
            int ret = write(filefd, buffer, size);//文件里面书写
            cout << ret << endl;
            memset(buffer, 0, sizeof(buffer));
            if (recvsize == filesize) { 
                break;
            }
        } 
    }
    cout << "开始向客户端发送文件" << endl;
    //需要找到数据库中的所有成员进行发送
    leveldb::Iterator* ita1 = db2->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    std::vector<std::string> str;
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == groupid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            auto s3 = nlohmann::ordered_json::parse(s2);
            str = s3["allmember"].get<std::vector<std::string>>();
            break;
        }
        ita1->Next();
    }
    delete ita1;
    //开始向客户端发送文件
    for (auto i = str.begin(); i != str.end(); i++) {
            if (online.count(*i) == 0) {
                continue;
            } 
            auto now = online.find(*i);
            auto targetfd = now->second;//找到目标fd之后
            if (targetfd == cfd) {
                continue;
            }
            write(targetfd, &file, sizeof(struct file_information));
            long long size1 ;
            long long sendsize = 0;
            long long r = 0;
            int filefd1 = open(filepath.c_str(), O_RDWR | O_CREAT | O_APPEND, 0644);
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
            //     cout << size1 << endl;
            //     perror("read");
            //     if (size1 > 0) {
            //         int re = write(targetfd, buffer, size1);
            //         cout << re << endl;
            //         memset(buffer, 0, sizeof(buffer));
            //     } else if (size1 == 0){
            //         printf("发送完成\n");
            //         close(filefd1);
            //         break;
            //     }
            // }
    }
    cout << "发送完毕" << endl;
    return 1;
}

//群聊消息的查看
int ghistory(int cfd, string& useid) {
    string message;
    int flag = 0;
    leveldb::Iterator* ita1 = db->NewIterator(leveldb::ReadOptions());
    ita1->SeekToFirst();
    while (ita1-> Valid()) {
        leveldb::Slice key = ita1->key();
        if (key.ToString() == useid) {
            leveldb::Slice s1 = ita1->value();
            string s2 = s1.ToString();
            flag = 1;
            message = s2;
            break;
        }
        ita1->Next();
    }
    delete ita1;
    if (flag == 0) {
        message = "无";
        sendMsg(cfd, message, strlen(message.c_str()));
        return 1;
    }
    sendMsg(cfd, message, strlen(message.c_str()));
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = cfd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[1];
    epoll_wait(epfd, evs, 1, -1);
    char *buff;
    recvMsg(cfd, &buff);
    string gid = buff;//记录需要查看的群id
    leveldb::Iterator *ita = db2->NewIterator(leveldb::ReadOptions());
    ita->SeekToFirst();
    string results;
    while (ita->Valid()) {
        leveldb::Slice key = ita->key();
        if (key.ToString() == gid) {
            //把内容拿出来转成josn格式才可以存储
            leveldb::Slice value = ita->value();
            auto s = value.ToString();
            auto jn = nlohmann::ordered_json::parse(s);
            vector<string> str;
            str = jn["history"].get<vector<string>>();
            for (auto i : str) {
                results = results + i + "\n";
            }
            break;
        } 
        ita->Next();    
    }
    delete ita;
    sendMsg(cfd, results, strlen(results.c_str()));
    return 1;    
}

