#include "epoll.h"
#include "funcc.h"
using namespace std;

//登陆功能的实现
int Login(int fd, string& temp1) {
    //第二个参数用来记录找到此人成功之后的id信息
    int cfd = fd;
    string s = "Login";
    
    sendMsg(cfd, s, 5);
    //将通信的文件描述符记录下来
    std::cout << "欢迎登陆小姚的聊天室" << "\n\n" << endl;
    string useid, password;
    printf("请输入你的账号：");
    cin >> useid;
    printf("请输入你的密码：");
    cin >> password;
    string buff;
    //接下来开始序列化
    int m = serialize(useid, password, buff);
    if (m == 0) {
        cout << "serialize fault" << endl;
    } else {
        cout << "serialize success" << endl;
    }

    //长度需要类型转换成为int
    int n = sendMsg(cfd, buff, strlen(buff.c_str()));
    if (n) {
        cout << "正在登陆中....." << endl;
        char *buff;
        sleep(0.1);
        recvMsg(cfd, &buff);
        string now = buff;
        free(buff);
        if (now == "YES") {
            //找到这个账号了
            cout << "恭喜您登录成功" << endl;
            temp1 = useid;//将登录人的所有信息全部记录
            return 1;
        } else if (now == "NO") {
            //不存在此账号
            cout << "不存在此账号" << endl;
            cout << "请返回上一层先进行注册" << endl;
            return 2;
        } else {
            cout << now << endl;
            return 3;
        }
    } 
        cout << "请求登陆失败" << endl;
        return -1;

}

//注册功能实现
int Sign_in(int fd,int number) {
    int cfd = fd;
    string s = "Sign_in";
    sendMsg(cfd, s, 7);
    string useid, password, usename, buff;
    string& buff1 = buff;//记录注册时候的所有信息
    while (1) {
        std::cout << "欢迎进入注册界面" << endl;
        std::cout << "请输入你要注册的账号:(最多8位)" << std::endl;
        std::cin >> useid;
        std::cout << "请输入你的密码:(最多10位)" << std::endl;
        std::cin >> password;
        if (useid.size() > 9 || password.size() > 11) {
            cout << "你输入的账号或者密码不符合要求，请重新输入" << std::endl;
            continue;
        } else {
            break;
        }
    }
    cout << "请输入你的昵称" << endl;
    cin >> usename;
    //接下来开始序列化
    int m = serialize1(useid, password, usename, buff1);
    if (m == 0) {
        cout << "serialize fault" << endl;
    } else {
        cout << "serialize success" << endl;
    }

    int n = sendMsg(cfd, buff, strlen(buff.c_str()));
    int flag = 0;
    if (n) {
        char *buff;
        sleep(0.1);
        recvMsg(cfd, &buff);
        if (strcmp("YES", buff) == 0) {
            std::cout << "恭喜您注册成功，请按回车键返回上一层登录" << std::endl;
            //cin >> s;
            flag = 1;  
        } else if(strcmp("NO", buff) == 0) {
            std::cout << "注册失败" << std::endl;
            flag = 0;
        }
    } 
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
    
}

//注销功能实现
int Logout(int fd, std::string example) {
    int cfd = fd;
    string s = "Logout";
    sendMsg(cfd, s, 6);
    string useid, buff;
    std::cout << "请输入你要注销的账号" << std::endl;
    cin >> useid;
    int m = serialize2(useid, buff);
    if (m == 0) {
        cout << "serialize3 fault" << endl;
    } else {
        cout << "serialize3 success" << endl;
    }

    int flag = 0;//记录是否成功的信息
    int n = sendMsg(cfd, buff, strlen(buff.c_str()));
    if (n) {
        char *buff;
        sleep(0.1);
        recvMsg(cfd, &buff);
        if (strcmp("YES", buff) == 0) {
            std::cout << "恭喜您注销成功，请按回车键返回上一层登陆" << std::endl;
            flag = 1;

        } else if(strcmp("NO", buff) == 0) {
            std::cout << "注销失败" << std::endl;
            flag = 0;

        }
    }
    if (flag == 1) {
        return 1;
    } else {
        return -1;
    }
}
    