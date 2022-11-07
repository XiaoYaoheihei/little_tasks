#include "worker.h"
using namespace std;

//测试用例
//测试自己创建的三个类是否有bug
int main() {
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