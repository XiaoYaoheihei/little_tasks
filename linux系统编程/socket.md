# 套接字——socket  

- 提出的一个背景：最初发明的计算机只能单独的运行自己的信息（比如计算之类），如果想要在多个计算机之间共享信息的话，只能是把内存条从一个计算机拿出来，然后放到目标计算机上。这样是不是很麻烦，所以大佬们就在想如何让不同的计算机之间接受，发送消息更加方便。最简单的想法肯定是把他们都相互连接起来呗（我也是这么想得，有点意思昂）。  
  然后他们就用某种方式把这些计算机都连接起来了（至于是哪种方式，好像是电缆的方式，下去可以查查康，这个东西感觉挺交叉的）。  
  连接成功之后，这些连接在一起的计算机之间就可以相互进行信息通信了。  
  实现这个通信的技术就叫做socket。

刚开始几个小型的计算机之间想要进行信息通信，肯定会采用信息传输，会用电缆（用来通信的）把这些计算机连接起来，然后通过电缆进行电信号的传输。  
想想，几个计算机之间就要使用电缆，会有点太费材料了，所以后来就发明了无限网络技术，不使用电缆了，通过无限电波技术来进行传输信息。  
猜测：三次握手是不是发生在传输层？传输层的任务是确立连接与断开连接还有重发。  

## 相关概念  

- 局域网：将一个区域里面的各种计算机、外部设备、数据库连接起来形成的一个巨大的私有网络。
- 广域网：不同地区之间局域网的连接。  
- IP：本质是一个整形数，用于表示计算机在网络中的地址。IP 协议版本有两个：IPv4 和 IPv6。（在通信的时候，我总得知道信息的传达地址在哪里吧，要不然我怎么发给你啊。）每一个计算机都有一个独立的IP，这样我就可以把信息发给你了。（具体的4和6的形式下去研究研究）  
- 端口：定位到主机上的某一个进程（专门用于完成某一个任务的进程），通过这个端口进程就可以接受到对应的网络数据了。```通过IP地址可以定位到某一台主机，通过端口就可以定位到主机上的某一个进程。``` ```通过指定的 IP 和端口，发送数据的时候对端就能接受到数据了。```  

```   
    端口也是一个整形数 unsigned short ，一个 16 位整形数，有效端口的取值
范围是：0 ~ 65535(0 ~ 216-1)
    提问：计算机中所有的进程都需要关联一个端口吗，一个端口可以被重复使用吗？
    不需要，如果这个进程不需要网络通信，那么这个进程就不需要绑定端口的.
    一个端口只能给某一个进程使用，多个进程不能同时使用同一个端口.
```

## 数据的封装以及网络协议：  

（我现在有了地址，有了端口，现在就差发送的数据了。）数据可以是自己电脑上的任何东西。为了保证数据传输过程中的标准化（大家都是依据这种方式来发送数据），研究了一种OSI模型（网络互联模型），即所有计算机之间的通信过程都是基于这个模型进行的。另外多台计算机通信过程中相互之间也必须遵守交换信息的规则，这种规则集合就被称作协议。  
数据封装：  

```
在网络通信的时候，程序猿需要负责的应用层数据的处理 (最上层)
    - 应用层的数据可以使用某些协议进行封装，也可以不封装.
    - 程序猿需要调用发送数据的接口函数，将数据发送出去.
    - 程序猿调用的 API 做底层数据处理
        - 传输层使用传输层协议打包数据
        - 网络层使用网络层协议打包数据
        - 网络接口层使用网络接口层协议打包数据
        - 数据被发送到 internet
    - 接收端接收到发送端的数据
        - 程序猿调用接收数据的函数接收数据
        - 调用的 API 做相关的底层处理:
            - 网络接口层拆包 ==> 网络层的包
            - 网络层拆包 ==> 网络层的包
            - 传输层拆包 ==> 传输层数据
    - 如果应用层也使用了协议对数据进行了封装，数据的包的解析需要程序猿做
```

## socket编程：  

  服务器与客户端。服务器可以让多个客户端进行访问，然后对多个客户端进行服务。  
  Q：在发送和接受数据的过程中，都是把封装好的数据存到内存之中，然后发送；接受也是一样，把内存中数据取出来然后解析。但是发送端和接收端在内存中处理数据的方式不一样的话，那么我们就无法正确解析出来想要的数据，怎么解决呢？

```
在各种计算机体系结构中，对于字节、字等的存储机制有所不同，因而引发了计算机通信领域中一个很重要的问题，即通信双方交流的信息单元（比特、字节、字、双字等等）应该以什么样的顺序进行传送。如果不达成一致的规则，通信双方将无法进行正确的编 / 译码从而导致通信失败。
```

- 字节序：  
  解决了前面我们提出的问题。  
  ```字节序，顾名思义字节的顺序，就是大于一个字节类型的数据在内存中的存放顺序，也就是说对于单字符来说是没有字节序问题的，字符串是单字符的集合，因此字符串也没有字节序问题。```  
  目前在各种体系的计算机中通常采用的字节存储机制主要有两种：Big-Endian 和 Little-Endian。  

****

  Little-Endian -> 主机字节序 (小端)：  
    数据的低位字节存储到内存的低地址位 , 数据的高位字节存储到内存的高地址位  
    我们使用的 PC 机，数据的存储默认使用的是小端。  

Big-Endian -> 网络字节序 (大端)  
    数据的低位字节存储到内存的高地址位 , 数据的高位字节存储到内存的低地址位  
    套接字通信过程中操作的数据都是大端存储的，包括：接收/发送的数据、IP地址、端口。

****

```uint16_t htons(uint16_t hostshort);	```  
将一个短整形从主机字节序 -> 网络字节序  
```uint32_t htonl(uint32_t hostlong);	```  
将一个整形从主机字节序 -> 网络字节序

- IP地址转换  

1) 主机字节序的ip转化成为网络字节序,主机字节序为字符串，而网络字节序为整形数据。  
   ```int inet_pton(int af, const char *src, void *dst); ```  

  ```
  参数:

    af: 地址族 (IP 地址的家族包括 ipv4 和 ipv6) 协议
        AF_INET: ipv4 格式的 ip 地址
        AF_INET6: ipv6 格式的 ip 地址
    src: 传入参数，对应要转换的点分十进制的 ip 地址: 192.168.1.100
    dst: 传出参数，函数调用完成，转换得到的大端整形 IP 被写入到这块内存中
  
  返回值：成功返回 1，失败返回 0 或者 - 1
  ```

    2) 网络字节序转化成为主机字节序（将大端的整形数转化成为小端的点十分进制ip）  

    ```const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);```

  ```
  参数:

    af: 地址族协议
        AF_INET: ipv4 格式的 ip 地址
        AF_INET6: ipv6 格式的 ip 地址
    src: 传入参数，这个指针指向的内存中存储了大端的整形 IP 地址
    dst: 传出参数，存储转换得到的小端的点分十进制的 IP 地址
    size: 修饰 dst 参数的，标记 dst 指向的内存中最多可以存储多少个字节

返回值:

    成功：指针指向第三个参数对应的内存地址，通过返回值也可以直接取出转换得到的 IP 字符串
    失败: NULL
  ```

- sockaddr数据结构  
  将IP和端口全部先转化成大端的顺序，然后再存储到结构体中。**所以就需要使用以上罗列出来的函数进行主机字节序和网络字节序之间的转换。**  

```c
  struct sockaddr {  
	sa_family_t sa_family;  // 地址族协议, ipv4  
    char   sa_data[14]; // 端口(2字节) + IP地址(4字节) + 填充(8字节)  
    };
```

```这个函数在写数据的时候不好用,所以一般使用下面的函数```  
```sizeof(struct sockaddr) == sizeof(struct sockaddr_in)``` 

``` c
struct in_addr
{
    in_addr_t s_addr;
}; 

  struct sockaddr_in  
{  
    sa_family_t sin_family;		/* 地址族协议: AF_INET */  
    in_port_t sin_port;         /* 端口, 2字节-> 大端  */  
    struct in_addr sin_addr;    /* IP地址, 4字节 -> 大端  */  
    /* 填充 8字节 */  
    unsigned char sin_zero[sizeof (struct sockaddr) - sizeof(sin_family) -
               sizeof (in_port_t) - sizeof (struct in_addr)];
};  
```

- ```套接字函数```  
  传入参数和传出参数的简单区别  

1) ```int socket(int domain, int type, int protocol);```  
   创建一个套接字  

  ```
参数:

    domain: 使用的地址族协议
        AF_INET: 使用 IPv4 格式的 ip 地址
        AF_INET6: 使用 IPv4 格式的 ip 地址
    type:
        SOCK_STREAM: 使用流式的传输协议
        SOCK_DGRAM: 使用报式 (报文) 的传输协议
    protocol: 一般写 0 即可，使用默认的协议
        SOCK_STREAM: 流式传输默认使用的是 tcp
        SOCK_DGRAM: 报式传输默认使用的 udp

返回值:
    成功：可用于套接字通信的文件描述符
    失败: -1
  ```

2) ```int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);```  
   将文件描述符和本地的IP绑定(中间的是传入参数)  

```
参数:

    sockfd: 监听的文件描述符，通过 socket () 调用得到的返回值
    addr: 传入参数，要绑定的 IP 和端口信息需要初始化到这个结构体中，IP和端口要转换为网络字节序
    addrlen: 参数 addr 指向的内存大小，sizeof (struct sockaddr)

返回值：成功返回 0，失败返回 - 1
```

3) ```int listen(int sockfd, int backlog);```  
   给监听的套接字设置监听  

```
参数:

    sockfd: 文件描述符，可以通过调用 socket () 得到，在监听之前必须要绑定 bind ()
    backlog: 同时能处理的最大连接要求，最大值为 128

返回值：函数调用成功返回 0，调用失败返回 -1
```

4) ```int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);```  
   等待并接受客户端的连接请求, 建立新的连接, 会得到一个新的文件描述符(通信的)(传出参数)		

```
参数:

    sockfd: 监听的文件描述符
    addr: 传出参数，里边存储了建立连接的客户端的地址信息
    addrlen: 传入传出参数，用于存储 addr 指向的内存大小

返回值：函数调用成功，得到一个文件描述符，用于和建立连接的这个客户端通信，调用失败返回 -1
```

5) ```ssize_t read(int sockfd, void *buf, size_t size);```  
   ```ssize_t recv(int sockfd, void *buf, size_t size, int flags);```  
    接受数据  
    **如果连接没有断开，接收端接收不到数据，接收数据的函数会阻塞等待数据到达，数据到达后函数解除阻塞，开始接收数据，当发送端断开连接，接收端无法接收到任何数据，但是这时候就不会阻塞了，函数直接返回0。** 

```
参数:

    sockfd: 用于通信的文件描述符，accept () 函数的返回值
    buf: 指向一块有效内存，用于存储接收是数据
    size: 参数 buf 指向的内存的容量
    flags: 特殊的属性，一般不使用，指定为 0

返回值:

    大于 0：实际接收的字节数
    等于 0：对方断开了连接
    -1：接收数据失败了
```

6) ```ssize_t write(int fd, const void *buf, size_t len);```  
   ```ssize_t send(int fd, const void *buf, size_t len, int flags);```  
   发送数据  

```
参数:

    fd: 通信的文件描述符，accept () 函数的返回值
    buf: 传入参数，要发送的字符串
    len: 要发送的字符串的长度
    flags: 特殊的属性，一般不使用，指定为 0

返回值：

    大于 0：实际发送的字节数，和参数 len 是相等的
    -1：发送数据失败了
```

7) ```int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);```  
   客户端请求和服务器端建立连接(传入参数)

```
参数:

    sockfd: 通信的文件描述符，通过调用 socket () 函数就得到了
    addr: 存储了要连接的服务器端的地址信息: iP 和 端口，这个 IP 和端口也需要转换为大端然后再赋值
    addrlen: addr 指针指向的内存的大小 sizeof (struct sockaddr)

返回值：连接成功返回 0，连接失败返回 - 1
```

## TCP网络通信 

通信之前，肯定是要在建立连接成功之后才是可以通信的。


```基于tcpde服务器端的通信代码：```  

```c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main() {
    //1.创建一个监听的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(0);
    }

    //2.和本地的IP地址以及端口绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;  //地址族协议
    addr.sin_port = htons(10000);//绑定的端口，一般5000以下的都归系统使用了
    addr.sin_addr.s_addr = INADDR_ANY;//绑定的本地的IP地址
    //这个INADDR_ANY代表的是本机所有的IP地址
    //这个宏可以表示任何一个IP地址
    //这个宏一般用于本地IP的绑定操作
    //宏的值为0 == 0.0.0.0
    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        perror("bind");
        exit(0);
    }

    //3.设置监听步骤
    ret = listen(fd, 128);
    if (ret == -1) {
        perror("listen");
        exit(0);
    }

    //4.阻塞并且等待客户端的连接
    struct sockaddr_in cliaddr;//把客户端的IP写到这个结构体里面
    int clilen = sizeof(cliaddr);
    int nfd = accept(fd, (struct sockaddr*)&cliaddr, &clilen);
    //新的通信描述符
    if (nfd == -1) {
        perror("accpet");
        exit(0);
    }
    //如果成功的话打印客户端的IP信息
    char ip[24] = {0};
    printf("客户端的IP地址为: %s, 端口为: %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
                                            ntohs(cliaddr.sin_port));
    //将IP和端口打印之前一定要把大端相应的数据转换成小端之后在打印

    //5.接受数据并且发送数据
    while (1) {
        //使用recv接受数据
        char buff[1024];//是为了把通信文件描述符对应的内核读缓冲区中的内容写进内存中
        int len = recv(nfd, buff, sizeof(buff), 0);
        if (len > 0) {
            printf("client say : %s\n", buff);
            send(nfd, buff, len, 0);//通信描述符所对应的写缓冲区中写对应的内容然后发送
        } else if (len == 0) {
            printf("客户端已经断开了连接。。。\n");
            break;
        } else {
            perror("recv");
            break;
        }
    }

    //6.关闭文件描述符
    close(fd);
    close(nfd);

    return 0;
}
```

```基于tcp通信的客户端通信代码```  

```c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main() {
    //1.创建通信的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(0);
    }

    //2.连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);   //端口转换成大端端口
    inet_pton(AF_INET, "192.168.1.100", &addr.sin_addr.s_addr);
    //ip地址转化成大端

    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        perror("connect");
        exit(0);
    }

    //3.和服务器之间进行通信
    int number = 0;
    while(1) {
        //发送数据
        char buf[1024];
        sprintf(buf, "你好，服务器。。。%d\n", number++);
        write(fd, buf, sizeof(buf));
        //往fd指向的内核缓冲区内进行写入

        //接受数据
        memset(buf, 0, sizeof(buf));//先把缓冲区内的数据清空再接受数据
        int len = read(fd, buf, sizeof(buf));
        if (len > 0 ) {
            printf("服务器say:%s\n", buf);
        } else if (len == 0) {
            printf("服务器断开了连接。。。\n");
        } else {
            perror("read");
            break;
        }
        sleep(1);   //每隔一秒发送并且接受一次数据
    }

    close(fd);
    return 0;
}
```
