#include "workManager.h"
#include "worker.h"
using namespace std;

workmanager::workmanager() {
    //初始化人数
    this->m_num = 0;
    //初始化数组指针
    this->m_array = NULL;

}

//展示菜单
void workmanager::show_menu() {
    cout << "********************************************" << endl;
	cout << "*********  欢迎使用职工管理系统！ **********" << endl;
	cout << "*************  0.退出管理程序  *************" << endl;
	cout << "*************  1.增加职工信息  *************" << endl;
	cout << "*************  2.显示职工信息  *************" << endl;
	cout << "*************  3.删除离职职工  *************" << endl;
	cout << "*************  4.修改职工信息  *************" << endl;
	cout << "*************  5.查找职工信息  *************" << endl;
	cout << "*************  6.按照编号排序  *************" << endl;
	cout << "*************  7.清空所有文档  *************" << endl;
	cout << "********************************************" << endl;
	cout << endl;
}

//退出功能
void workmanager::exitsystem() {
    cout << "欢迎下次使用" << endl;
    system("pause");
    exit(0);
}

//添加职工
void workmanager::add_emp() {
    cout << "请输入你要添加的职工数量" << endl;
    int addnum = 0;
    cin >> addnum;
    if (addnum > 0) {
        //计算新的空间的大小
        int newnum = this->m_num + addnum;
        //利用二维数组开辟新空间
        //一维指针是worker类（为了统一指向）
        worker ** newspace = new worker*[newnum];

        //将原来空间的内容存放到新空间里
        if (this->m_array != NULL) {
            for (int i = 0; i < this->m_num; i++) {
                //地址进行拷贝
                newspace[i] = this->m_array[i];
            }
        }
        //输入新的数据
        for(int i = 0; i < addnum; i++) {
            int id, did;
            string name;
            cout << "请分别输入第" << i+1 << "个员工的编号，姓名(使用回车符)" << endl;
            cin >> id;
            cin >> name;
            cout << "请选择该职工的岗位(编号)" << endl;
            cout << "1.普通职工" << endl;
            cout << "2.经理 " << endl;
            cout << "3.老板 " << endl;
            cin >> did;
            worker * now = NULL;
            switch (did) {
                case 1:
                    now = new employee(id, name, 1);
                    break;
                case 2:
                    now = new manager(id, name, 2);
                    break;
                case 3:
                    now = new boss(id, name, 3);
                    break;
                default://如果输入的不是对应的序号需要有对应的处理
                    break;
            }

            newspace[this->m_num + i] = now;
        }

        //释放原有的空间
        delete[] this->m_array;
        
        //更改空间指向
        this->m_array = newspace;

        //更新数量
        this->m_num = addnum;
        cout << "恭喜你添加成功一名职工" << endl;

    } else {
       cout << "输入的数量有问题" << endl;
    }
    system("pause");
    system("clear");
}


// workmanager::~workmanager() {

// }