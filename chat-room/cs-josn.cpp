#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include "json.hpp"
using namespace std;

namespace member {
class User {
public :
    std::string useid;  //id
    std::string usename;//昵称
    std::string password;//密码
    std::string friends;//好友

    //成员函数
    //默认构造函数
    User() = default;
    User(std::string& useid1,std::string& password1, std::string& usename1):useid(useid1), password(password1), usename(usename1) {}
    User(std::string& useid1,std::string& password1):useid(useid1), password(password1) {}
    //当把账号和密码传进来的时候
    //调用此成员函数进行初始化

    //构造函数
    //string to->josn
    static void To_josn(nlohmann::ordered_json& jn, const User& user) {
        jn = nlohmann::ordered_json {
            {"useid", user.useid},
            {"usename", user.usename},
            {"password", user.password},
            {"friends", user.friends}
        };
    }


    //josn to->string
    static void From_Json (const nlohmann::ordered_json& jn, User& user) {
        jn.at("useid").get_to(user.useid);
        jn.at("usename").get_to(user.usename);
        jn.at("password").get_to(user.password);
        jn.at("friends").get_to(user.friends);
    }
};
}

//客户端登陆时账号和密码的序列化
int serialize(string& useid1, string& password1, string& current) {
    string s1 = useid1;
    string s2 = password1;
    string jn_1= R"(
                {
                    "useid":"11111111",
                    "password":"11111111"
                }
                )";
    //解析  
    //printf("oojoj\n");
    auto jn_2 = nlohmann::ordered_json::parse(jn_1);
    jn_2["useid"] = s1;
    jn_2["password"] = s2;
    //nlohmann::ordered_json& jn = jn_2; 
    //cout << std::setw(2) << jn_2 << endl;
    //进行初始化
    //将传进来的id和word进行赋值
    //member::User user(useid1, password1);
    //member::User::To_josn(jn, user);
    //将传来的string进行解析
    current = to_string(jn_2);
    
    return 1;
}
//服务端登陆时候的解析
int analysis(char *buff, string& s1, string& s2) {
    //先将char*转化成为string类型
    string s = buff;
    //进行解析,而且必须是具有josn格式的字符串才可以
    auto jn_2 = nlohmann::ordered_json::parse(s);
    //cout << std::setw(2) << jn_2 << endl;
    s1 = jn_2["useid"].get<string>();
    s2 = jn_2["password"].get<string>();

    //cout << s1 << s2 << endl;
    return 1;
}

//客户端注册时候的序列化
int serialize1(string& useid1, string& password1, string& usename1, string& buff1) {
    auto jn_1= R"(
                {
                    "useid":"11111111",
                    "usename":"yeye",
                    "password":"11111111",
                    "friends":[

                    ],
                    "group":[
                        
                    ],
                    "sheild_f":[
                        
                    ]
                    
                }
                )";
    //解析  
    auto jn_2 = nlohmann::ordered_json::parse(jn_1);
    jn_2["useid"] = useid1;
    jn_2["password"] = password1;
    jn_2["usename"] = usename1;
    //nlohmann::ordered_json& jn = jn_2; 
    //cout << std::setw(2) << jn_2 << endl;
    //进行初始化
    //将传进来的id和word进行赋值
    //member::User user(useid1, password1, usename1);
    //member::User::To_josn(jn, user);
    //将传来的string进行解析
    
    buff1 = to_string(jn_2) ;
    return 1;
}
//服务端注册时候的解析
int analysis(char *buff, string& s11, string& s22, int number) {
    //先将char*转化成为string类型
    string s = buff;
    //进行解析
    auto jn_2 = nlohmann::ordered_json::parse(s);
    //cout << std::setw(3) << jn_2 << endl;
    auto s3 = jn_2["useid"].get<string>();
    s11 = s3;
    s22 = to_string(jn_2);

    // cout << s11 << "\n" << s22 << endl;
    return 1;
}


//客户端注销时候的序列化
int serialize2(string& useid1, string& buff1) {
    string jn_1= R"(
                {
                    "useid":"11111111"
                }
                )";
    //解析  
    string s = useid1;
    auto jn_2 = nlohmann::ordered_json::parse(jn_1);
    jn_2["useid"] = s;
    buff1 = to_string(jn_2);
    return 1;
}
//服务其注销时候的解析
int analysis(char *buff, string& s11) {
    //先将char*转化成为string类型
    string s = buff;
    //进行解析
    auto jn_2 = nlohmann::ordered_json::parse(s);
    s11 = jn_2["useid"].get<string>();

    return 1;
}

//客户端查看好友时的序列化
int analyse(string& buff, vector<string>& list1) {
    string s = buff;   
    //cout << s << endl;
    auto jn_2 = nlohmann::ordered_json::parse(s);
    //cout << jn_2 << endl;//进行检验，如果可以进行转换，将内容打印出来
                        //已经序列化成功了
    list1 = jn_2["friends"].get<vector<string>>();
    return 1;
}


//客户端添加好友，查看消息时的序列化
int analyse(string& buff, string& sendid) {
    string s = buff;    //不知道此时可否进行序列化，牵扯到一个序列化文本问题
    //cout << s << endl;
    auto jn_2 = nlohmann::ordered_json::parse(s);
    //cout << jn_2 << endl;//进行检验，如果可以进行转换，将内容打印出来

    sendid = jn_2["useid"].get<string>();
    return 1;
}

//服务器增添，删除好友时，进行聊天时id的序列化
int analys(string& info1, string& useid1) {
    string s = info1;
    //cout << s << endl;
    auto jn_2 = nlohmann::ordered_json::parse(s);
    //cout << jn_2 << endl;//进行检验，如果可以进行转换，将内容打印出来
    useid1 = jn_2["useid"].get<string>();
    return 1;

}

//客户端查看群组时的序列化
int group_analyse(string buff, vector<string> &list) {
    string s = buff; 
    auto jn_2 = nlohmann::ordered_json::parse(s);
    list = jn_2["group"].get<vector<string>>();
    return 1;
}
