## 进程控制

进程控制块以及`task_struct`结构



## 内核+中断环境+运行环境





## 进程同步

进程间的两种基本关系：**直接制约关系**和**间接制约关系**。

**允许许多进程同时访问的资源称为临界资源，这段资源上下的代码称为临界区。**

同步原语：

- 信号量

整型信号量

记录型信号量（高效的解决了临界资源的访问问题）：

​	signal()操作，对信号量+1再判断，<=0唤醒

​	wait()操作，对信号量-1再判断，<0阻塞

AND型信号量

信号量集

- 信号量机制的应用策略

**寻找临界资源；每种临界资源定义信号量；访问临界资源前使用前申请；使用后释放**

### 经典的进程同步问题

**笼统地来说，进程间地同步就是把wait和signal操作分别放在生产者和消费者地两个进程/线程中；而互斥是把临界区放在wait和signal操作之间。**

处理思路：同步模型——>进程同步分析——>同步控制——>代码验证

- 哲学家进餐问题





- 生产者-消费者问题

多生产者-多消费-单缓冲区（书上给的是多缓冲区，我的理解是单个缓冲区可以放多个数据，并不是书上给的单个缓冲区存放的单个 数据）：

- 缓冲区中资源的位置如果需要确定的话，需要记录生产的位置（in指针），消费的位置（out指针）；可以使用内核的缓冲区也可以使用自己定义的缓冲区，内核缓冲区不需要自己手动控制
- 临界资源的确定：缓冲区，位置

**生产者—消费者两者之间的同步关系，两者对共享资源的互斥访问。**

**当多线程之间有同步关系的话，一般要保证多进程对共享资源互斥访问的同时，同时还要采取相应的措施（信号量等）保证线程之间的同步。**

Q：能不能buffer不互斥访问，pro和con进入到buffer中后再判断能否存放和消费？



多生产-多消费-缓冲池（多buffer）：



- 读者-写者问题

读-读允许同时；读-写互斥；写-写互斥

## 进程间通信





## 处理机调度

调度概念：一堆任务要处理，由于资源有限。需要**确定某种规则来决定处理这些任务的顺序。**

### 三个层次

#### 高级调度——作业调度

作业：**一个具体的任务**。

从外存的作业后备队列中挑选一个作业调入内存，并且创建进程和其对应的PCB。每个作业只会被调入一次，调出一次。作业调出时会撤出PCB。

#### 低级调度——进程调度/处理机调度

按照某种策略从就绪队列中选取一个进程，将CPU资源分配给这个进程。

进程调度是OS中最基本的一种调度，一般的OS中都必须配置进程调度。

进程调度的频率很高。

#### 中级调度——内存调度

背景知识：当我们的内存不够的时候，可将某些进程的数据暂时调到外存中。等内存空闲的时候或者进程需要运行的时候再重新调入内存中。（切换程序的时候会感觉到有一丝卡顿）

暂时调到外存等待的进程状态是挂起状态。被挂起的进程PCB会被组织成挂起队列。

定义：按照某种策略决定将哪个处于挂起状态的进程重新调入内存。

一个进程会被多次调出和调入内存中，因此中级调度发生的频率比高级调度要高。

#### 三层调度之间的联系：

![三层调度联系以及对比](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E4%B8%89%E5%B1%82%E8%B0%83%E5%BA%A6%E8%81%94%E7%B3%BB%E4%BB%A5%E5%8F%8A%E5%AF%B9%E6%AF%94.png)

#### 进程状态新增——挂起状态

运行态进程可以直接到就绪挂起，创建态进程也可以直接到就绪态挂起。

**注意挂起和阻塞的区别**

![进程七状态模型](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E8%BF%9B%E7%A8%8B%E4%B8%83%E7%8A%B6%E6%80%81%E6%A8%A1%E5%9E%8B.png)

![七状态模型补充](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E4%B8%83%E7%8A%B6%E6%80%81%E6%A8%A1%E5%9E%8B%E8%A1%A5%E5%85%85.png)

### 调度时机，调度算法作用，调度过程的主要操作（进程的切换过程）

#### 调度时机

进程主动放弃CPU资源

- 正常终止
- 发生异常终止
- 进程主动阻塞

被动放弃

- 进程的时间片使用完毕
- IO中断等紧急事件需要处理
- 更高优先级的进程进入到就绪队列中

ps：不能进行进程调度与切换的情况：

- 处理中断的过程中
- 在进行原子操作的过程中（原语）
- 进程在**OS内核程序临界区**内（是内核临界区）
  - 内核程序临界区一般是用来访问某种内核数据结构的。举个例子：进程加载到就绪队列中的时候是在内核中进行的，一般进行此过程需要加锁，但是如果这个时候进程调度的话，其他进程又无法从就绪队列中拿出进程，影响到内核的其他管理。因此不能进行调度与切换。
  - 普通临界区资源不会影响OS内核的管理工作，所以是可以进行调度切换的。

#### 进程调度方式

##### 非抢占式（非剥夺资源方式）

**只允许进程主动放弃处理机资源，即使有紧急任务也不会分配，直到进程终止或者主动要求进入阻塞状态。**

实现简单，适合早期的批处理系统。

##### 抢占式（剥夺资源方式）

有紧急任务的时候，会停止当前将处理机分配给更重要的进程。

优先处理更加紧急的进程，也可以实现让各种进程按照时间片流转执行相应的功能。适合分时系统或者实时系统。

#### 进程的切换与过程

![进程切换](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E8%BF%9B%E7%A8%8B%E5%88%87%E6%8D%A2.png)

主要完成了两个任务：当前进程保存现场，加载进程加载现场。

会把程序计数器，状态字，各种寄存器等信息保存到PCB当中去。

进程切换是有代价的，如果过于频繁的进行进程调度和切换，会使得效率降低。

#### 调度程序/调度器

![调度程序](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E8%B0%83%E5%BA%A6%E7%A8%8B%E5%BA%8F.png)

支持的粒度是进程

支持的粒度是线程

![线程调度](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%BA%BF%E7%A8%8B%E8%B0%83%E5%BA%A6.png)

### 周转时间，平均周转时间，带权周转时间

周转时间=完成时间-到达时间

等待时间=周转-运行

带权周转时间=周转/运行

平均周转=周转的和/个数

### 几大调度算法

算法思想，算法规则，算法属于作业调度还是进程调度，抢占式还是非抢占式，优点和缺点，是否会导致饥饿。

#### 先来先服务（FCFS）

用于作业调度，考虑的是哪个作业先到达后备队列

用于进程调度，考虑哪个进程先到达就绪队列

**重点考虑的是等待时间**。

![先来先服务](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%85%88%E6%9D%A5%E5%85%88%E6%9C%8D%E5%8A%A1.png)

![FCFS](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/FCFS.png)

#### 短作业优先（SJF）--默认的是非抢占式

**抢占式的称为：最短剩余时间优先算法（SRTN），不是学校考察的范围，但是比SJF在某些指标上更加高效**

最短的作业/进程首先得到服务（服务时间最短）

每次调度选择**当前已经到达**并且**运行时间最短**的进程/作业。

**重点考虑的是执行时间**。

![短作业优先](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%9F%AD%E4%BD%9C%E4%B8%9A%E4%BC%98%E5%85%88.png)

![SJF](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/SJF.png)

#### 高响应比优先（HRRN）

为了避免上面两种单一的考虑，此算法**综合考虑等待时间和执行时间。**

![高响应比优先](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%AB%98%E5%93%8D%E5%BA%94%E6%AF%94%E4%BC%98%E5%85%88.png)

![HRRN](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/HRRN.png)

#### 时间片轮转（RR）

按照进程到达就绪队列的顺序，轮流让各个进程执行一个时间片（时钟中断来产生中断信号）。**主要是应用于进程调度**。

时间片太大，会退化成FCFS，因此不能太大，会**增大进程响应时间**。时间片太小，会导致进程切换过于频繁，系统会有开销。

![时间片轮转](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%97%B6%E9%97%B4%E7%89%87%E8%BD%AE%E8%BD%AC.png)

![RR1](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/RR1.png)

![RR2](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/RR2.png)

![RR3](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/RR3.png)

![RR4](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/RR4.png)

#### 优先级调度算法

每个进程/作业都有自己的优先级，调度的时候选择优先级最高的进程/作业。——优先数和优先级依据题目。

![优先级调度](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E4%BC%98%E5%85%88%E7%BA%A7%E8%B0%83%E5%BA%A6.png)

![非抢占的优先级调度](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%9D%9E%E6%8A%A2%E5%8D%A0%E7%9A%84%E4%BC%98%E5%85%88%E7%BA%A7%E8%B0%83%E5%BA%A6.png)

![抢占式优先级调度](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%8A%A2%E5%8D%A0%E5%BC%8F%E4%BC%98%E5%85%88%E7%BA%A7%E8%B0%83%E5%BA%A6.png)

补充信息：

静态优先级：创建进程之后就不会改变。

动态优先级：创建进程的时候会有一个初始值，会根据情况动态调整。

![补充优先级调度](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E8%A1%A5%E5%85%85%E4%BC%98%E5%85%88%E7%BA%A7%E8%B0%83%E5%BA%A6.png)

#### 多级反馈队列算法思想（不在学校期末考试范围内）

Unix采用的方式

## 死锁问题

### 什么是死锁问题

在并发环境下，各个进程因为资源竞争而造成的一种互相等待对方手里的资源，导致各个进程都处于阻塞态，无法向前推进的现象就是“死锁”。

![死锁，饥饿，死循环区别](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%AD%BB%E9%94%81%EF%BC%8C%E9%A5%A5%E9%A5%BF%EF%BC%8C%E6%AD%BB%E5%BE%AA%E7%8E%AF%E5%8C%BA%E5%88%AB.png)

### 死锁问题产生的原因（必要条件）

**互斥条件**：对互斥使用的资源争抢会导致死锁。

**不剥夺条件**：进程获得的资源在未使用完之前，不能由其他进程强行夺走，只能主动释放。

**请求和保持条件**：进程已经保持了至少一个资源，但是又提出了新的资源要求。

**循环等待条件**：存在一种进程资源的循环等待链，链中的每一个进程已经获得资源的同时还会等待下一个进程的请求。

发生死锁的时候一定由循环等待，但是发生循环等待未必死锁。

### 死锁的处理策略

#### 预防死锁：

破坏四种条件中的一个或者几个。

1.把互斥资源改造成允许共享使用。比如spooling技术。

2.当进程请求的资源得不到满足的时候，必须立即释放当前资源；当进程需要的资源被占用的时候，可以由OS协助，将资源强行剥夺。

3.**静态分配方法**：进程在运行之前一次申请完所有资源，在资源未满足之前不让他投入运行，一旦运行这些资源就会一直归他所有。

4.**顺序资源分配法**：给系统中的资源编号，规定每一个进程必须按照编号递增的顺序请求资源，同类资源（编号相同的）一次性申请完。

小编号可以申请大编号，大编号不可以申请小编号。---》不会导致全部进程都死锁。

#### 避免死锁：

安全序列，不安全状态，安全状态

![避免死锁的概念](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%81%BF%E5%85%8D%E6%AD%BB%E9%94%81%E7%9A%84%E6%A6%82%E5%BF%B5.png)

银行家算法

#### 死锁的检测和解除：

死锁检测算法

死锁解除算法

### 银行家算法设计思想

![银行1](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%93%B6%E8%A1%8C1.png)

![银行2](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%93%B6%E8%A1%8C2.png)

## 内存概述和连续分配

### 内存概述

#### 存储层次是具有结构性，存储结构中都包含有哪些类存储

寄存器，Cache，内存，外存，远程存储

从左往右速度变慢，成本下降，容量提升

#### 内存空间如何实现保护

提出了系统区和用户区，分别存放不同的代码。

系统区存放内核代码，用户区存放用户代码。

**为了防止越界，通过硬件检查程序是否越界——基址寄存器（起始物理地址）和限长寄存器（最大逻辑地址）**

![存储保护](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%AD%98%E5%82%A8%E4%BF%9D%E6%8A%A4.png)

#### 深入理解什么是地址重定位，为什么要设计地址重定位？动态重定位和静态重定位的含义。理解程序如何装入内存？

逻辑地址：逻辑上的地址。

物理地址：在内存中的实际地址。

地址重定位（地址映射/地址变化）：将逻辑地址转化成物理地址。

装入地址是从0开始，装入不是从0开始。

绝对装入：在编译的时候就将产生绝对地址。

![绝对装入](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%BB%9D%E5%AF%B9%E8%A3%85%E5%85%A5.png)

静态重定位：将装入模块装入内存中的时候再对地址进行重定位。

![静态装入](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%9D%99%E6%80%81%E8%A3%85%E5%85%A5.png)

动态重定位：装入的时候还是逻辑地址，真正执行程序的时候才会进行地址转化。需要一个**重定位寄存器**支持。

![动态重定位](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%8A%A8%E6%80%81%E9%87%8D%E5%AE%9A%E4%BD%8D.png)

#### 了解程序是如何进行链接？

![静态编译](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%9D%99%E6%80%81%E7%BC%96%E8%AF%91.png)

![装入时链接](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E8%A3%85%E5%85%A5%E6%97%B6%E9%93%BE%E6%8E%A5.png)

![运行时动态链接](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E8%BF%90%E8%A1%8C%E6%97%B6%E5%8A%A8%E6%80%81%E9%93%BE%E6%8E%A5.png)

#### 内存如何进行扩充大小——引入虚拟内存

### 内存管理

#### 内存空间的分配与回收

##### 连续分配管理方式——为用户进程分配的必须是一个连续的内存空间。

- 单一连续分配

![单一连续分配](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%8D%95%E4%B8%80%E8%BF%9E%E7%BB%AD%E5%88%86%E9%85%8D.png)

- 固定分区分配

整个用户空间划分成若干个固定大小的分区。

![固定分区分配](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%9B%BA%E5%AE%9A%E5%88%86%E5%8C%BA%E5%88%86%E9%85%8D.png)

建立一个数据结构——分区说明表来实现对各个分区的分配与回收。每一个表项包括对应分区的大小，起始地址，状态（是否已经被分配）。

优点：无外部碎片。

缺点：程序太大时分区可能不会满足需求；会产生内部碎片。

- 动态分区分配

  不会预先划分内存分区，而是在进程装入内存的时候根据进程的大小动态地建立分区，并且使得分区的大小正好适合进程的需要。

  - 数据结构记录内存的使用情况

    ![空闲分区数据结构](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%A9%BA%E9%97%B2%E5%88%86%E5%8C%BA%E6%95%B0%E6%8D%AE%E7%BB%93%E6%9E%84.png)

  - 多个空闲分区满足条件的时候，选择哪个分区进行分配。

    **动态分区分配算法**

  - 如何进行分区的分配与回收操作？

​					取决于不同的数据结构和不同的分配算法，对空闲分区表的处理或者是对空闲分区链表进行处理。

回收区的后面有一个空闲分区；回收区的前面有一个空闲分区；回收区的前后都有一个空闲分区；回收区的前后都没有空闲分区——相邻的全部合并。

##### 动态分区分配算法

###### FF——首次适应算法

每次都从**低地址**开始查找，找到第一个能满足大小的空闲分区。空闲分区是按照地址从低到高的次序排列。

第一次来15MB，第二次来8MB。

![FF](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/FF.png)

###### NF——循环首次适应

从上次查找结束的位置开始查找。

不需要对链表进行重新排列，链表是循环链表的方式。

![NF](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/NF.png)

###### BF——最佳适应

找容量最小满足的，第一次来9MB,第二次来3MB

会产生很多外部碎片。

![BF](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/BF.png)

###### WF——最坏适应

找容量最大满足的。

第一次来3MB，第二次来9MB。

缺点：会导致较大的连续空闲区被迅速用完。

![WF](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/WF.png)

产生了外部碎片之后，通过**紧凑**（compaction）的技术来解决外部碎片技术。

动态重定位对程序的位置进行改变，程序的位置改变之后对程序的起始地址（基址寄存器）进行修改。

##### 非连续分配管理方式



#### 从逻辑上对内存空间的扩展

##### 覆盖技术

为了解决程序大小超过物理内存总和的问题。

缺点：由程序员声明覆盖结构，对用户不透明。

![覆盖技术](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E8%A6%86%E7%9B%96%E6%8A%80%E6%9C%AF.png)

![覆盖技术1](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E8%A6%86%E7%9B%96%E6%8A%80%E6%9C%AF1.png)

##### 交换技术（中级调度/内存调度）

内存中的某些进程暂时换到外存中，把外存中的某些进程换入内存。

PCB会常驻内存，不会被换出外存。

![交换技术三个问题](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E4%BA%A4%E6%8D%A2%E6%8A%80%E6%9C%AF%E4%B8%89%E4%B8%AA%E9%97%AE%E9%A2%98.png)

##### 虚拟存储技术

#### 内存地址转化

三种装入方式（绝对装入时转化）

### 内存保护

前面有笔记

## 存储器管理

早期存储管理不足：

- 程序需要**一次性全部装入内存**中
- 随着多个程序的启动和结束，内存会出现**外部碎片**
- 碎片无法全部利用，通过**拼凑合并**，代价很大

如何进行优化：

### 页式存储管理（32位机器下）

每个进程**逻辑上**分为相同大小的页（4k），通过**页表**映射到具**体的物理内存块**，页表中存储的是（逻辑地址页号）和（物理地址块号）。

内存中按照页的大小分为大小相等的块，物理地址块号。

程序将虚拟地址传给CPU，CPU通过MMU计算**虚拟地址**对应的**页号以及在页内的偏移量**（根据页号直接计算出虚拟地址的前多少位是页号，后多少位是偏移量）

31                   12   11                             0

-----------|-------------

​    		页号p	  |		页内偏移量    

-----------|-------------

#### 管理页表和内存

计算进程需要多少块，分配相应的物理地址空间。

系统为**每个进程都分配了一个页表**，页表的**起始地址和长度**填入到PCB，并且存入到内存中。

#### 地址变换机构（MMU）

输入为逻辑地址，输出为物理地址

#### 具有快表（TLB）的地址转化机构——页表缓存

缓存页表中频繁访问的局部区域（页表子集）

如果每次MMU访问TLB命中的话，需要更新TLB中的值。

#### 如何进行地址转化？

页面大小直接计算出来位数。

页号=逻辑地址/页面长度

页内偏移量=逻辑地址%页面长度

**千万记得还要判断页号是否越界**

![img](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%A1%B5%E8%A1%A8%E8%AE%A1%E7%AE%97.png)



#### 多级页表

计算机支持非常大的逻辑地址空间，**页表会很长**，需要内存大的**连续空间**存放页表，为每一个**进程都分配这么大的页表**放到内存中真的很不现实，所以，页表切割成离散分布，且仅存放当前需要的部分页表项。

- 页表分割成页，在内存中离散分布
- 程序的局部性原理，将**当前需要的页调入内存，其余页驻留在磁盘中，需要的时候再调用。**

二级页表工作原理：

- **页内地址**，直接放到MMU中的偏移位置，用于最后的拼接
- **外部页号**，通过外部页号查找到对应的页号在内存中的块号（页表号所在的块号）
- **内层页号**，通过块号中的页表和**内层页号**（每一块中都放着一页）查找出来对应的内存块号，

31                                       21                                         11                                       0

-----------------|----------------|---------------|

 一级页号（页目录号）   二级页号 （页表索引）  页内偏移量

-----------------|----------------|---------------|

#### 如何进行地址转化？

根据一级拿到块号中的页表，根据二级拿到相应的内存块号，根据内存块号和页内偏移量计算出最终要访问的物理地址。

![img](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E4%B8%80%E7%BA%A7%E9%A1%B5%E8%A1%A8%E5%92%8C%E4%BA%8C%E7%BA%A7%E9%A1%B5%E8%A1%A8.png)

#### 需要注意的问题：

1.如果采用多级页表机制，则**各级页表的大小不能超过一个页面**。

![两级页表注意](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E4%B8%A4%E7%BA%A7%E9%A1%B5%E8%A1%A8%E6%B3%A8%E6%84%8F.png)

2.两级页表的**访存次数分析（假设此时没有快表）：一共需要访问三次内存**；但是一级页表需要访问两次内存就可以了。

## 虚拟存储器

### 为什么要引入虚拟内存器？

程序本身所占内存是4G，但是物理内存可能只有2G，明显的物理内存不够用。

#### 分析问题：

我们的程序一般都是必须全部加载到内存当中去才可以执行，这种**一次性装入**和**运行时的驻留性**导致了问题的出现。

#### 程序两大局部性原理：

时间局部性(某一条指令可能会被循环执行，for循环语句)和空间局部性(某一存储单元被使用，在一段时间内与该存储单元邻近的单元也会被使用)

### 工作原理

动态请求调入+置换+存储器管理方式（分页和分段）

当进程运行时, 先将**一部分程序装入内存**, 另一部分暂时留在外存, 当要执行的指令不在内存时, 由系统自动将它们从外存调换到内存。即具有请求调入和置换功能。

虚拟存储器定义: 具有**请求调入功能和置换功能**，能仅把作业的一部分装入内存便可运行作业的存储器系统，它能从逻辑上对内存容量进行扩充的一种虚拟的存储器系统。

### 工作情况

- 只需要把**部分程序的代码和数据**放入到内存中，其他部分保存在外存中
- 需要时刻保证内存与外存的**动态切换**

### 实现方式：

硬件支持：中断处理机构（专门用来处理缺页/缺段）+请求分页的页表机构（修改用来适应）+请求分页的地址变化机构

#### 请求分页的管理方式

- 请求分页的页表机制（页表中新增了几个部分）

  |页号|块号|状态位|访问位|修改位|外存地址|

  **状态位P**：指示该页是否调入内存，供程序访问时参考

  **访问位**：记录该页在一段时间内被访问的次数，供置换算法选择换出页面时参考。

  **修改位**：查看此页是否在内存中被修改过，供置换页面时是否回写外存参考。

  **外存地址**：外存物理块号，供调入该页时参考。

- 缺页中断处理过程

  **保留CPU现场**；从外存中找到缺页；判断内存是否满页，如果不是满页的话，CPU从外存中读取页，如果是满页的话，选择一页换出，并让CPU从外存中读取页；启动IO硬件，将一页从外存中换入到内存中；修改页表；**恢复CPU现场**。

- 地址变换机构工作流程

  左上角开始
  
  ![地址变换机构流程](E:\os图片\地址变换机构流程.png)

### 页面的内存分配

内存分配策略概念：1）为进程分配多少内存物理块     2）置换页的策略

分配物理块策略：	1）固定分配										2）可变分配

置换页的策略：		1）局部置换(置换当前的进程页)	2）全局置换(置换所有的进程页)

排列组合之后常用的方式：可变分配+全局置换（已经使用在若干OS中）

### 页面的调入策略

#### 何时调入

预调

需求调入

#### 何处调用

对换区（已经被访问的程序和数据在swap区）

文件区（未被访问的程序和数据在文件区）

#### 调入流程（和之前请求分页中的地址变换机构工作流程一部分相同）

![地址变换机构流程](E:\os图片\地址变换机构流程.png)

#### 好玩的东西：

对二维数组不同的的双重for循环赋值方法，性能方面的差距还是挺大的。但是编译器会对此进行优化处理。

```c
int a[18][18];
//横向赋值
for (int i = 0; i < 18; i++) {
	for(int j = 0; j < 18; j++) {
		a[i][j] = 23;
    }
}

//纵向赋值
for (int i = 0; i < 18; i++) {
	for (int j =0; j < 18; j++) {
        a[j][i] = 23;
    }
}
```

### 页面置换算法

定义：选择换出内存中的哪个页面。      

好的页面置换算法应具有低的页面更换频率（缺页率是一种近似表示）  **缺页率 =（缺页次数/访问总页数）*100%**

#### 最佳置换算法——OPT

![OPT](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/OPT.png)

此算法可以保证最低的缺页率，但是实际上只有在执行的过程中才能知道接下来要访问哪一个页面。OS无法提前知道页面的访问序列，因此此**算法无法实现**。

#### 先进先出算法——FIFO

![FIFO](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/FIFO.png)

#### LRU算法

![LRU](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/LRU.png)

#### CLOCK算法——LRU近似算法（NRU）

扫描的指针会指向当前置换页的下一个页面。

![CLOCK](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/CLOCK.png)

#### 改进型的CLOCK算法

![改进的CLOCK](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%94%B9%E8%BF%9B%E7%9A%84CLOCK.png)

![改进的CLOCK1](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%94%B9%E8%BF%9B%E7%9A%84CLOCK1.png)

## 输入输出系统（IO设备）

CPU给相应的硬件中的寄存器（向控制器中的）写入命令，硬件根据指令具体操控设备。硬件执行完毕之后，向CPU发出中断信号。CPU执行中断处理。

OS要给用户提供一个简单的视图——文件视图。（为了让out指令做的统一，形成一个统一的文件接口）

**终端设备包括显示器和键盘。**

### 显示器

一段操作外设的程序

**不同的设备文件名字表示不同的设备。**

### 键盘——中断信号部分

敲键盘就会产生中断。

故事的开始是从中断初始化开始，中断初始化程序。

程序操作的是一个write_queue队列或者是read_queue队列。

### 缓冲（cpu与设备之间）

为什么要使用缓冲？

哪几种缓冲？——单缓冲区，双缓冲区，缓冲池。

在哪里存放着？——机械设备，设备控制器，内存中都有。

由谁管理，怎样管理？——OS管理

#### 缓冲设计的基本思想：

暂存交换数据，用于处理速度不匹配的问题。

#### IO缓冲方式：

- 单缓冲

  ![单缓冲0](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%8D%95%E7%BC%93%E5%86%B20.png)

  ![单缓冲](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%8D%95%E7%BC%93%E5%86%B2.png)

  ![单缓冲2](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%8D%95%E7%BC%93%E5%86%B22.png)

  **采用单缓冲，处理一块数据块的平均耗时是Max(C,T)+M**

- 双缓冲（缓冲对Buffer）

  ![双缓冲](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%8F%8C%E7%BC%93%E5%86%B2.png)

  ![双缓冲1](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%8F%8C%E7%BC%93%E5%86%B21.png)

  **采用双缓冲，处理一个数据块的平均耗时是Max(T,C+M);与此同时也可以粗略地估计为Max(T,C)**

- 缓冲池（设计很巧妙）

### 设备管理

#### IO设备的基本概念和分类

IO设备就是可以将数据输入到计算机或者可以接受计算机输出数据的外部设备。

**Unix系统将外部设备抽象成一种特殊的文件，用户可以使用与文件操作相同的方式对外部设备进行操作。**

按照使用特性分类：

- 人机交互外部设备：鼠标，键盘等（数据传输最慢）
- 存储设备：移动硬盘，光盘等（数据传输最快）
- 网络通信设备：路由器等

#### IO控制器

IO设备一般都分为机械部件和电子部件，**CPU是通过电子部件来实现对机械部件的操作**。这个电子部件就是IO控制器。

功能：

- 接收和识别CPU发出的指令：控制寄存器中会存放命令和对应的参数
- 向CPU报告设备状态：状态寄存器会记录此时设备的状态
- 数据交换：数据寄存器暂存CPU发来的数据或者设备数据
- 地址识别：为各个寄存器编址，通过地址判断是哪个寄存器
  - 为各个寄存器编址的时候采用内存映射IO或者寄存器独立编址
    - 内存映射IO，控制器中寄存器与内存地址统一编址
    - 寄存器独立编址：寄存器单独编址

一个IO控制器会对应多个设备；寄存器会有多个以此对应多个设备的相应数据。

![控制器编址](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%8E%A7%E5%88%B6%E5%99%A8%E7%BC%96%E5%9D%80.png)

#### IO控制方式

注意的问题：

1.完成一次读写操作的流程	2.CPU干预的频率	3.数据传送的单位	4.数据的流向	5.主要的缺点和优点

- 程序直接控制方式

  - 轮询的具体的操作

  - **CPU干预很频繁**，轮询的方式

  - 每次读写一个**字**

  - 数据流向：IO<->CPU<->内存

    ![程序直接控制方式](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%A8%8B%E5%BA%8F%E7%9B%B4%E6%8E%A5%E6%8E%A7%E5%88%B6%E6%96%B9%E5%BC%8F.png)

  ![程序直接控制方式2](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%A8%8B%E5%BA%8F%E7%9B%B4%E6%8E%A5%E6%8E%A7%E5%88%B6%E6%96%B9%E5%BC%8F2.png)

- 中断驱动方式

  - 引入中断驱动方式，CPU发出指令之后，将等待IO的进程阻塞，去执行其他的进程。当IO进程完成之后发出中断信号，CPU检测到中断信号，保护现场，转而去处理该中断。处理中断的过程中，**CPU从控制器中读取一个字到CPU的数据寄存器再写入到主存当中**，接着恢复IO进程的运行环境，继续执行。

  - 注意的点：

    - CPU会在**每个指令周期末尾**检查中断

    - 中断处理过程中需要保护现场，这个过程是需要一定的时间开销的。如果中断发生频率太高也会降低系统性能。

      

- DMA方式（直接存储器存取）

  - 为了直接绕过CPU而衍生的一种技术

  - **数据传送的单位不再是字，而是块**

  - 数据的流向从设备直接放入到内存中

  - 仅仅在传送一个块或者多个数据块的开始和结束的时候需要CPU的干预

  - **CPU发出指令，DMA控制器接受指令之后，开始读写操作，完成之后发出中断信号。需要注意的是DMA控制器也是每次读取一个字放到DC中，然后将DC中的数据放入到内存中。最后完成的结果呈现出来的是传送单位是块。**

    ![DMA控制器](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/DMA%E6%8E%A7%E5%88%B6%E5%99%A8.png)

- **通道控制方式**

  - 通道一种硬件，可以理解成弱鸡版的CPU。**通道可以识别并且执行一系列的通道指令**。

    ![通道控制方式](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%80%9A%E9%81%93%E6%8E%A7%E5%88%B6%E6%96%B9%E5%BC%8F.png)

四种方式对比：

![四种IO方式对比](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%80%9A%E9%81%93%E6%8E%A7%E5%88%B6%E6%96%B9%E5%BC%8F.png)

#### IO软件的层次结构（一定要看！）

![层次结构](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%B1%82%E6%AC%A1%E7%BB%93%E6%9E%84.png)

##### 用户层软件：

**向上提供了库函数**

实现了与用户交互的接口

##### 设备独立性软件：（系统调用处理层）

**向上提供了系统调用**，read之类的

功能：

- 提供设备保护，设备是特殊的文件
- 差错处理
- 设备的分配与回收，设备是一种临界资源
- **数据缓冲区管理**
- **建立逻辑设备名到物理设备名之间的映射关系，根据设备的类型选择调用相应的驱动程序**
- ![逻辑设备](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%80%BB%E8%BE%91%E8%AE%BE%E5%A4%87.png)

##### 设备驱动程序：

**完成对硬件的具体操作控制，将上层发出的硬件指令转化成特定设备听得懂的一系列操作**

##### 中断处理程序：

进行中断处理，也会和硬件直接打交道

![中断处理程序](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E4%B8%AD%E6%96%AD%E5%A4%84%E7%90%86%E7%A8%8B%E5%BA%8F.png)

##### 硬件：

不同厂家的设备具有不同的硬件特性

#### IO核心子系统

重点掌握：IO调度，设备保护，设备回收与管理，缓存区管理（前面四种都是在设备独立性软件层实现），假脱机技术

- IO调度：使用某种算法确定一个好的顺序处理各个IO请求
- 设备保护：OS实现文件保护功能，设备时特殊的文件

#### Spooling技术——单设备虚拟化成多设备：假脱机技术

**用户层软件实现**

脱机：脱离主机的控制进行的输入与输出。

![假脱机](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%81%87%E8%84%B1%E6%9C%BA.png)



**输入井和输出井都是在磁盘中的。**

共享打印机原理分析：

![共享打印机](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E5%85%B1%E4%BA%AB%E6%89%93%E5%8D%B0%E6%9C%BA.png)

虽然系统中只有一台打印机，但是每一个进程提出打印请求的时候系统都会在输出井中为其分配一个存储区，使得每个进程都感觉自己独占了一台设备。

#### 设备的分配与回收



#### 磁盘管理

物理概念

一次disk读/写操作需要的时间：

**寻找时间**：磁头移动到固定磁道所花的时间。

**延迟时间**：通过旋转磁盘，使得磁头定位到目标扇区需要的时间。

**传输时间**：读出/写入数据所经历的时间。

##### 磁盘调度（IO调度）：磁道请求安排合理响应顺序

- 先来先服务策略（FCFS）

  ![磁道FCFS](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%A3%81%E9%81%93FCFS.png)

- 最短寻道时间优先(SSTF)——造成“饥饿”现象，不公平，一般用作参考值

  优先处理的磁道是与当前磁头最近的磁道。可以保证每次的寻道时间最短，但是不能保证总的寻道时间最短。

  ![磁道SSTF](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%A3%81%E9%81%93SSTF.png)

- 扫描算法SCAN（电梯调度算法）（经常使用）

  只有磁头移动到最外侧磁道的时候才可以向内移动，移动到最内侧磁道的时候才可以往外移动。（有方向性）

  ![磁道SCAN](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%A3%81%E9%81%93SCAN.png)

- LOOK调度算法（解决SCAN其中的一个缺点）

  ![磁道LOOK](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%A3%81%E9%81%93LOOK.png)

- 循环扫描算法CSCAN（解决SCAN其中的一个缺点）

  ![磁道CSCAN](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%A3%81%E9%81%93CSCAN.png)

​		

磁盘冗余阵列

1.并行交叉存取

2.

- RAID 2
- RAID 3
- RAID 5

通过磁盘校验值的方式可以恢复数据：（恢复数据的方式又多了一种方式）

前提是1*3=3，现在我只知道其中一个乘数和结果，那我肯定可以根据某种规则推算出来其中另一个数据的具体结果。



## 文件系统

驱动程序把盘块转化成具体的磁盘块，文件系统层抽象成**盘块**来看待。

open()过程干了什么事情，他的意义是什么？

### 文件管理

#### 文件内部的数据组织起来的方式：

- **无结构文件——由一些二进制或者字符流组成（典型txt文件中），又称“流式文件”。**
- 有结构文件——由一组相似的记录组成，又称“记录式文件”，记录中的每一项称为数据项。

文件之间的组织方式：用户通过创建目录将文件之间有序的组织起来。

#### 文件应该如何存放在外存：

外存类似于内存，也是由一个个的存储单元组成的。

外存是按照块为一个存储单元的。

**OS也是以块为单位为文件分配存储空间的**，即使一个文件只有1b，但是他依然是需要占用1kb的磁盘块大小。外存中数据读入内存的时候也是以块为单位。

![文件存放](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%96%87%E4%BB%B6%E5%AD%98%E6%94%BE.png)

![文件存放1](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%96%87%E4%BB%B6%E5%AD%98%E6%94%BE1.png)

### 文件的逻辑结构

所谓的**逻辑结构，就是指在用户看来，文件内部的数据是应该如何组织起来的。**而物理结构指的是在OS看来，文件的数据是如何存放在外存中的。

#### 无结构文件

#### 有结构文件

##### 顺序文件

文件中记录一个接一个地顺序排列。

![顺序文件](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%A1%BA%E5%BA%8F%E6%96%87%E4%BB%B6.png)

![顺序文件随机查找](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%A1%BA%E5%BA%8F%E6%96%87%E4%BB%B6%E9%9A%8F%E6%9C%BA%E6%9F%A5%E6%89%BE.png)

##### 索引文件

为了解决可变长记录文件中随机查找某一个记录的需求，建立了一张索引表来加快文件检索速度。

![索引文件](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%B4%A2%E5%BC%95%E6%96%87%E4%BB%B6.png)

##### 索引顺序文件

他会为一个文件建立一张索引表，但是不会为每一个记录建立一个索引表项，**而是一组记录对应一个索引表项**。

检索效率分析以及检索过程分析：

**首先通过索引表查找到分组，找到分组之后再在分组中顺序查找记录。**——这种设计思路很妙也很常见。

![检索效率分析](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%A3%80%E7%B4%A2%E6%95%88%E7%8E%87%E5%88%86%E6%9E%90.png)

### 文件目录

#### FCB（文件控制块）——文件的数据结构

**FCB的有序集合称为“文件目录”**，一个FCB就是一个文件目录项。FCB记录着文件的某些属性信息。

树形目录结构：实现了递归查找。

**缺点：树形结构不便于实现文件的共享。**

一层一层通过每一个目录的FCB集合信息，查找到需要进入的目录项之后，从DISK中调入新的FCB集合信息，接着查找。

![FCB](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/FCB.png)

#### Inode（索引节点）

我们按照文件名进行查询的时候其实并不需要除了文件名之外的所有信息，可以将这些信息统一存放到另外的位置，然后**使用一个索引结点进行指向。**

![索引结点](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%B4%A2%E5%BC%95%E7%BB%93%E7%82%B9.png)

![索引结点补充](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E7%B4%A2%E5%BC%95%E7%BB%93%E7%82%B9%E8%A1%A5%E5%85%85.png)

### 文件的物理结构（文件分配方式）——十分重要

![文件块](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%96%87%E4%BB%B6%E5%9D%97.png)

对磁盘块进行管理的方式：

#### 对非空闲块磁盘块管理——存放了文件数据

##### 文件的分配方式——文件数据应该怎样存放在外存中

- 连续分配

![连续分配](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E8%BF%9E%E7%BB%AD%E5%88%86%E9%85%8D.png)

优点：支持顺序访问和直接访问（随机访问）；连续分配的的文件在顺序读/写时速度最快。

缺点：不方便对文件进行拓展。

- 链接分配

为文件分配了离散的磁盘块。

隐式链接：**逻辑块号转化成物理块号的过程中需要读磁盘操作。**

![隐式链接分配](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E9%9A%90%E5%BC%8F%E9%93%BE%E6%8E%A5%E5%88%86%E9%85%8D.png)

优点：方便文件进行拓展

显式链接：**逻辑块号转化成物理块号的过程中不需要读磁盘操作，只需要通过FAT表就可以知道物理块号信息。**

![显式链接分配](https://github.com/XiaoYaoheihei/little_tasks/blob/main/os%E8%AF%BE%E7%A8%8B%E4%BB%A3%E7%A0%81/os%E5%9B%BE%E7%89%87/%E6%98%BE%E5%BC%8F%E9%93%BE%E6%8E%A5%E5%88%86%E9%85%8D.png)

- 索引分配



