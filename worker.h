//创建一个抽象类
//具体的岗位通过继承实现
#pragma once
#include<iostream>
#include<string>
using namespace std;

//抽象类
class worker
{
public:
    //纯虚函数
    //展示个人信息
    virtual void showinfo() = 0;
    //获取岗位名称
    virtual string getdeptname() = 0;

    int m_id;       //职工编号
    string m_name;  //职工名字
    int m_deptid;   //所在部门编号
   
};

//员工类
class employee: public worker
{
public:
    //构造函数
    employee(int id, string name, int did);

    //virtual关键字可删可不删
    virtual void showinfo();
    virtual string getdeptname();
};

//经理类
class manager: public worker
{
public:
    manager(int id, string name, int did);

    virtual void showinfo();
    virtual string getdeptname();
};

//老板类
class boss: public worker 
{
public:
    boss(int id, string name, int did);
    virtual void showinfo();
    virtual string getdeptname();
};