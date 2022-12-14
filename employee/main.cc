#include "workManager.h"
using namespace std;
#include <iostream>
#include <string>
#define EXIT    choice == "0"
#define ADD     choice == "1"
#define DELETE  choice == "2"
#define SHOW    choice == "3"
#define MODIFY  choice == "4"
#define FIND    choice == "5"
#define SORT    choice == "6"
#define CLEAR   choice == "7"

int main() {
    //首先实例化一个对象
    workmanager now;
    string choice;

    while(true) {
        now.show_menu();
        cout << "请输入您的选择" << endl;
        cin >> choice;
        //switch语句只能匹配int和emum类型的数据
        //对于string类型的数据不能用switch语句
        // switch (choice)
        // {}
        if (EXIT) { //退出
           now.exitsystem();
           break;
        } else if (ADD) {//添加
            now.add_emp();
            // test_add();     //测试添加功能
            break;
        } else if (DELETE) {//删除
            now.de_emp();
            break;
        } else if (SHOW) {//显示
            now.show_emp();
            break;
        } else if (MODIFY) {//修改
            now.modify_emp();
            break;
        } else if (FIND) {//查找
            now.find_emp();
            break;
        } else if (SORT) {//排序
            now.sort_emp();
            break;
        } else if (CLEAR) {//清空
            now.clear_emp();
            break;
        } else {
            system("cls");
        }
    }
    
    system("pause");
    return 0;
}