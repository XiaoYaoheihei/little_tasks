#include "workManager.h"
#include "worker.h"
using namespace std;

workmanager::workmanager() {
    ifstream ifs;
    //读文件的模式
    ifs.open(FILENAME, ios::in);

    //文件不存在的情况下
    if (!ifs.is_open()) {
        cout << "文件不存在" << endl;
        //初始化人数
        this->m_num = 0;
        //初始化数组指针
        this->m_array = NULL;
        //设置文件不存在的标志
        this->m_fileempty = true;
        ifs.close();
        return;
    } 
    char ch;
    ifs>>ch;
        //函数只读到了文件的结尾标志
        if (ifs.eof()) {
            cout << "文件为空" << endl;
            this->m_num = 0;
            //初始化数组指针
            this->m_array = NULL;
            //设置文件不存在的标志
            this->m_fileempty = true;
            ifs.close();
            return;
        }
        int num = get_num();
        //计算成员人数
        this->m_enum = num;
        
        //分配成员数组空间
        this->m_array = new worker *[this->m_enum];
        //初始化员工数组
        this->init_emp();

        //测试是否初始化成功
        // test_init(this->m_array, this->m_enum);

        ifs.close();
        return;

    }

//初始化员工数组
void workmanager::init_emp() {
    ifstream ifs;
	ifs.open(FILENAME, ios::in);

	int id;
	string name;
	int dId;
	int index = 0;
	while (ifs >> id && ifs >> name && ifs >> dId)
	{
		worker * work = NULL;
		//根据不同的部门Id创建不同对象
		if (dId == 1)  // 1普通员工
		{
			work = new employee(id, name, dId);
		}
		else if (dId == 2) //2经理
		{
			work = new manager(id, name, dId);
		}
		else //总裁
		{
			work = new boss(id, name, dId);
		}
		//存放在数组中
		this->m_array[index++] = work;
	}
    ifs.close();
}

//统计文件中的人数
int workmanager::get_num() {
    ifstream ifs;
	ifs.open(FILENAME, ios::in);

	int id;
	string name;
	int did;

	int num = 0;

	while (ifs >> id && ifs >> name && ifs >> did)
	{
        //记录人数
		num++;
	}
	ifs.close();

	return num;
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
    //初始化的时候要添加的人数为0
    this->m_num = 0;
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

        //更新员工的数量
        this->m_num = addnum;
        cout << "恭喜你添加成功一名职工" << endl;
        //更改职工不为空的标志
        this->m_fileempty = false;

        //测试添加成功之后的保存文件
        this->save();

    } else {
       cout << "输入的数量有问题" << endl;
    }
    system("pause");
    system("clear");
}

//将信息保存到文件中去
void workmanager::save() {
    ofstream ofs;
    //以写的方式打开文件
    ofs.open(FILENAME, ios::out);
    //将每个人的数据写入文件中去
    for(int i = 0; i < this->m_num; i++) {
        ofs << this->m_array[i]->m_id << " "
            << this->m_array[i]->m_name << " " 
            << this->m_array[i]->m_deptid << endl;
    }

    ofs.close();
}

//显示职工信息
void workmanager::show_emp() {
    if (this->m_fileempty) {
        cout << "文件为空" << endl;
    }
    for (int i = 0; i < this->m_enum; i++) {
        //利用多态接口调用直接实现
        this->m_array[i]->showinfo();
    }
}


workmanager::~workmanager() {
    if (this->m_array != NULL) {
        delete[] this->m_array;
    }
}