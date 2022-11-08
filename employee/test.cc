#include "worker.h"
using namespace std;

//测试用例
//测试自己创建的三个类是否有bug
void test_add() {
    worker * work = NULL; 
    work = new employee(1, "张三", 1);
    work->showinfo();
    delete work;

    work = new manager(2, "李四", 2);
    work->showinfo();
    delete work;

    work = new boss(3, "王五", 3);
    work->showinfo();
    delete work;    
}

//测试是否初始化员工成功
void test_init(worker** m_array, int num) {
    for(int i = 0; i < num; i++) {
        cout << m_array[i]->m_id << " "
            << m_array[i]->m_name << " "
            << m_array[i]->m_deptid << endl;
    }
}