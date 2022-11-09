#pragma once
#include <iostream>
#include <fstream>
#include "worker.h"
#define FILENAME "empfile.txt"
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
    ~workmanager();
    void show_menu();   //显示信息
    void show_emp();    //显示职工信息
    void exitsystem();  //退出程序

    int m_num;          //记录添加的人数个数
    worker** m_array;   //员工数组的指针
                        //开辟新的空间用worker*来作为数组的元素
    void add_emp();     //添加新的员工
    
    int m_enum;         //员工总数
    void save();        //将员工信息保存到文件
    bool m_fileempty;   //标志文件是否为空
    int get_num();      //统计文件中的人数
    void init_emp();    //初始化员工

    int isexit(int id); //通过编号来判断职工是否存在
    void de_emp();      //删除职工

    void modify_emp();  //修改职工

    void find_emp();    //查找职工
    void find_way(int&);  //按照编号进行查找
    void find_way(string&);//按照姓名进行查找

    void sort_emp();    //对员工进行排序

    void clear_emp();   //清空文件
};

