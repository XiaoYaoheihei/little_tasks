#include "worker.h"

//员工类初始化
employee::employee(int id, string name, int did) {
    this->m_id = id;
    this->m_name = name;
    this->m_deptid = did;
}
//打印信息
void employee::showinfo() {
    cout << "职工编号" << this->m_id << endl;
    cout << " \t职工姓名: " << this->m_name << endl;
	cout << " \t岗位:" << this->getdeptname() << endl;
	cout << " \t岗位职责:完成经理交给的任务" << endl;
}
//获取职位信息
string employee::getdeptname() {
    return string("员工");
}


//经理类的初始化
manager::manager(int id, string name, int did) {
    this->m_id = id;
    this->m_name = name;
    this->m_deptid = did;
}
void manager::showinfo() {
	cout << "职工编号： " << this->m_id
		<< " \t职工姓名: " << this->m_name
		<< " \t岗位:" << this->getdeptname()
		<< " \t岗位职责:完成老板交给的任务,并下发任务给员工" << endl;
}
string manager::getdeptname() {
	return string("经理");
}


//老板类的初始化
boss::boss (int id, string name, int did) {
    this->m_id = id;
    this->m_name = name;
    this->m_deptid = did;
}
void boss::showinfo()
{
	cout << "职工编号： " << this->m_id
		<< " \t职工姓名: " << this->m_name
		<< " \t岗位:" << this->getdeptname()
		<< " \t岗位职责:管理公司所有事务" << endl;
}
string boss::getdeptname()
{
	return string("总裁");
}