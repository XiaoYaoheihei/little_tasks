#pragma once
#include <iostream>
#include "worker.h"
using namespace std;
/*
是为了整体实现一个管理类（将打开一个系统看成是实例化一个类对象）
具体的操作包括：
展示与用户的沟通菜单界面，
对职工增删改查的操作，
与文件的读写交互
*/

//管理类
class workmanager {
public:
    workmanager();      //构造函数
    void show_menu();   //显示信息
    void exitsystem();  //退出程序
    int m_num;          //记录人数个数
    worker** m_array;   //员工数组的指针
                        //开辟新的空间用worker*来作为数组的元素
    void add_emp();         //添加新的员工
    ~workmanager();
};

