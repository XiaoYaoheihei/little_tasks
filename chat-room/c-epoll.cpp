#include "epoll.h"
#include "funcc.h"
using namespace std;

int main() {
    int fd = createSocket();
    if (fd == -1) {
        return -1;
    }
    // 设置端口复用
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    int ret = connetToHost(fd, "127.0.0.1", 10000);
    if (ret == -1) {
        return -1;
    }
    //成功与服务器连接之后，需要获得此时本地所绑定的端口号
    struct sockaddr_in my;
    unsigned int len = sizeof(my);
    int r = getsockname(fd, (struct sockaddr*)&my, &len);
    if (r == -1) {
        perror("errno");
        return 1;
    }
    //成功建立连接之后就按照逻辑进行登陆
    string choice;
    while (1) {
        cout << "   1.      登陆界面        " << endl;
        cout << "   2.      注册界面        " << endl;
        cout << "   3.      注销界面        " << endl;
        cout << "   0.      退出界面        " << endl;
        cin >> choice;
        if (choice == "0") {
            
            break;
        } else if (choice == "1") {
            string temp; //记录登录人的id信息
            string& temp1 = temp;   //传进去
            int result = Login(fd, temp1);
            
            if (result == 1) {
                cout << "进行消息fd" << endl;
                pthread_t real_time;//设计实时消息进行通知
                pthread_create(&real_time, NULL, inform, &my);
                pthread_t detach(real_time);
                //登陆成功，进入主页面
                int re = enter(fd, temp1, 1, 1);
                if (re == 1) {
                    continue;
                }
            } else if(result == -1) {
                //系统出现异常
                std::cout << "请求登陆异常" << std::endl;
                break;
            } else if (result == 2) {
                //进入注册界面
                continue;
            } else if (result == 3) {
                cout << "此账号正处于登录状态，你不能登录" << endl;
                continue;
            }
        } else if (choice == "2") {
            int result = Sign_in(fd, 1);
            if (result == 1) {
                continue;
            } else if(result == -1) {
                std::cout << "注册失败" << std::endl;
                break;
            }
        } else if (choice == "3") {
            //注销界面
            int result = Logout(fd, "nonono");
            if (result == 1) {
                std::cout << "注销成功" << std::endl;
                continue;
            } else if(result == -1) {
                std::cout << "注销失败" << std::endl;
                break;
            }
        }
    }
    
    printf("接下来的功能呢\n");

    close(fd);

    return 0;
}
//实时消息的通知
void* inform(void * argc) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int ret = connetToHost(fd, "127.0.0.1", 10000);
    char *buff;
    while (1) {
        recvMsg(fd, &buff);//不断地接收通知的消息
        printf("%s\n", buff);
        if (strcmp(buff, "exit") == 0) {
            break;
        }
    }
    free(buff);
    return NULL;
}

int enter(int fd, string& buff, int number, int n) {
    //第一个参数是通信描述符，第二个参数是记录的登录人的id信息
    string choice;
    while (1) {
        cout << "     欢迎进入主界面            " << endl;
        cout << "       1.好友界面              " << endl;
        cout << "       2.群聊界面              " << endl;
        cout << "       3.接收添加消息界面          " << endl;
        cout << "       0.退出登录            " << endl;
        cin >> choice;
        if (choice == "1") {
            int n = friends(fd, buff, 1);
            if (n == 1) {
                continue;
            } else{
                exit(1);
            }
        } else if (choice == "2") {
            int n = group(fd, buff);
            if (n == 1) {
                continue;
            } else {
                exit(1);
            }
        } else if (choice == "3") {
            int n = messageadd(fd, buff, "no");
            if (n == 1) {
                continue;
            } else {
                exit(1);
            }
        } else if (choice == "0") {
            string now = "Quit";
            sendMsg(fd, now, strlen(now.c_str()));
            break;
        } else {
            continue;
        }
    }
    return 1;
}

//消息添加界面
int messageadd(int cfd, string& buff, string a1) {
    string choice;
    while (1) {
        cout << "       0.返回上一层            " << endl;
        cout << "       1.好友添加界面          " << endl;
        cout << "       2.群聊添加界面          " << endl;
        cin >> choice;
        if (choice == "1") {
            int n = messagemember(cfd, buff, "no");
        } else if (choice == "2") {
            int n = messagegroup(cfd, buff);
        } else if (choice == "0") {
            break;
        } else {
            continue;
        }
    }
    return 1;
}