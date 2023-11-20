## 1、心跳包

### 一、存在下面两种情形：

情形一：一个客户端连接服务器以后，如果长期没有和服务器有数据来往，可能会被防火墙程序关闭连接，有时候我们并不想要被关闭连接。例如，对于一个即时通讯软件，如果服务器没有消息时，我们确实不会和服务器有任何数据交换，但是如果连接被关闭了，有新消息来时，我们再也没法收到了，这就违背了“即时通讯”的设计要求。

情形二：通常情况下，服务器与某个客户端一般不是位于同一个网络，其之间可能经过数个路由器和交换机，如果其中某个必经路由器或者交换器出现了故障，并且一段时间内没有恢复，导致这之间的链路不再畅通，而此时服务器与客户端之间也没有数据进行交换，由于 TCP 连接是状态机，对于这种情况，无论是客户端或者服务器都无法感知与对方的连接是否正常，这类连接我们一般称之为“死链”。

### 二、解决方式

情形一中的应用场景要求必须保持客户端与服务器之间的连接正常，就是我们通常所说的“保活“。如上文所述，当服务器与客户端一定时间内没有有效业务数据来往时，我们只需要给对端发送心跳包即可实现保活。

情形二中的死链，只要我们此时任意一端给对端发送一个数据包即可检测链路是否正常，这类数据包我们也称之为”心跳包”，这种操作我们称之为“心跳检测”。顾名思义，如果一个人没有心跳了，可能已经死亡了；一个连接长时间没有正常数据来往，也没有心跳包来往，就可以认为这个连接已经不存在，为了节约服务器连接资源，我们可以通过关闭 socket，回收连接资源。

根据上面的分析，心跳检测一般有两个作用：

保活 、检测死链

### 三、TCP keepalive 选项

操作系统的 TCP/IP 协议栈其实提供了这个的功能，即 keepalive 选项。在 Linux 操作系统中，我们可以通过代码启用一个 socket 的心跳检测（即每隔一定时间间隔发送一个心跳检测包给对端），代码如下：

```c
//on 是 1 表示打开 keepalive 选项，为 0 表示关闭，0 是默认值
int on = 1;
setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on));
```

但是，即使开启了这个选项，这个选项默认发送心跳检测数据包的时间间隔是 7200 秒（2 小时），这时间间隔实在是太长了，不具有实用性。

我们可以通过继续设置 keepalive 相关的三个选项来改变这个时间间隔，它们分别是 TCP_KEEPIDLE、TCP_KEEPINTVL 和 TCP_KEEPCNT，示例代码如下：

```c
//发送 keepalive 报文的时间间隔
int val = 7200;
setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val));
 
//两次重试报文的时间间隔
int interval = 75;
setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
 
int cnt = 9;
setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &cnt, sizeof(cnt));
```

TCP_KEEPIDLE 选项设置了发送 keepalive 报文的时间间隔，发送时如果对端回复 ACK。则本端 TCP 协议栈认为该连接依然存活，继续等 7200 秒后再发送 keepalive 报文；如果对端回复 RESET，说明对端进程已经重启，本端的应用程序应该关闭该连接。
如果对端没有任何回复，则本端做重试，如果重试 9 次（TCP_KEEPCNT 值）（前后重试间隔为 75 秒（TCP_KEEPINTVL 值））仍然不可达，则向应用程序返回 ETIMEOUT（无任何应答）或 EHOST 错误信息。

我们可以使用如下命令查看 Linux 系统上的上述三个值的设置情况：

```
[root@iZ238vnojlyZ ~]# sysctl -a | grep keepalive
net.ipv4.tcp_keepalive_intvl = 75
net.ipv4.tcp_keepalive_probes = 9
net.ipv4.tcp_keepalive_time = 7200
```

### 四、应用层的心跳包机制设计

由于 keepalive 选项需要为每个连接中的 socket 开启，这不一定是必须的，可能会产生大量无意义的带宽浪费，且 keepalive 选项不能与应用层很好地交互，因此一般实际的服务开发中，因此通常在应用层设计自己的心跳包机制。

从技术来讲，心跳包其实就是一个预先规定好格式的数据包，在程序中启动一个定时器，定时发送即可，这是最简单的实现思路。但是，如果通信的两端有频繁的数据来往，此时到了下一个发心跳包的时间点了，此时发送一个心跳包。这其实是一个流量的浪费，既然通信双方不断有正常的业务数据包来往，这些数据包本身就可以起到保活作用，所以不必浪费流量去发送这些心跳包。所以，对于用于保活的心跳包，最佳做法就是设置一个上次包时间，每次接收和发送数据时都更新一下这个包时间，而心跳检测计时器每次检测时，将这个包时间与当前系统时间做一个对比，如果时间间隔大于最大时间间隔（15 - 45秒不等），则发送一次心跳包。总之，就是在与对端之间，没有数据来往达到一定时间间隔时才发送一次心跳包。

> 需要注意的是：一般是客户端主动给服务器发送心跳包，服务器做心跳检测决定是否断开连接。服务器再收到心跳包时应该回应一个心跳应答。

```c++
/*
	客户端代码。保活
*/
#include <iostream>
#include <cstring>
#include <thread>
#include <string.h>
#include <chrono>
#include <mutex>
#include <stdio.h>
#ifdef _WIN32
//Windows
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")
#else 		
//Linux平台
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

using namespace std;

// 定义消息类型（心跳数据 + 用户数据）
struct Message
{
	int type;          // 消息类型。0心跳包，1用户数据
	char heartData[8]; // 心跳包数据
	char name[36];     // 姓名
	int age;           // 年龄
	char gender;       // 性别
};

// 全局变量，记录上一次发送心跳包的时间
int last_heartbeat_time = time(NULL);

// 互斥锁，保护全局变量
std::mutex heartbeat_time_mutex;

// 建立与服务端的连接，并返回套接字
int connectServer(const string &ip, const int &port)
{
#ifdef _WIN32
	// 创建套接字
	SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		perror("socket");
		exit(-1);
	}

	// 连接到服务器（这里封装的数据都是服务器的ip和端口）
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(ip.data());
	addr.sin_port = htons(port);
	int ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret == -1)
	{
		perror("connect");
		exit(-1);
	}

	return fd;
#else
	// 创建套接字
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		perror("socket");
		exit(-1);
	}

	// 连接到服务器（这里封装的数据都是服务器的ip和端口）
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &(addr.sin_addr.s_addr));
	addr.sin_port = htons(port);
	int ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret == -1)
	{
		perror("connect");
		exit(-1);
	}

	return fd;
#endif
	
}

// 发送心跳包并接收响应数据
void sendHeartBeat(int sockfd)
{
	while (true)
	{
		// 判断是否需要发送心跳包
		// 对last_heartbeat_time枷锁
		std::unique_lock<std::mutex> lock(heartbeat_time_mutex);
		int esc = time(NULL) - last_heartbeat_time;
		// 解锁
		lock.unlock();

		// 如果超过15秒未给服务器发送数据，就会发送心跳包维持连接
		if (esc >= 15)
		{
			// 发送心跳包
			Message heartbeat = { 0, "BEAT", "", 0, '\0' };
#ifdef _WIN32
			send(sockfd, (char *)&heartbeat, sizeof heartbeat, 0);
#else
			write(sockfd, &heartbeat, sizeof heartbeat);
#endif // _WIN32

			// 对last_heartbeat_time枷锁
			std::unique_lock<std::mutex> lock(heartbeat_time_mutex);
			// 更新时间
			last_heartbeat_time = time(NULL);
			// 解锁
			lock.unlock();
		}
		else
		{
			// 延迟
			this_thread::sleep_for(chrono::seconds(1));
		}
	}
}

// 发送用户并接收响应数据
void sendData(int sockfd)
{

	while (true)
	{
		// 读取用户输入
		char name[36] = { 0 };
		int age;
		char gender;

		std::cout << "请输入姓名: " << std::endl;
		std::cin >> name;
		std::cout << "请输入年龄: " << std::endl;
		std::cin >> age;
		std::cout << "请输入性别(m/f): " << std::endl;
		std::cin >> gender;
		std::cin.clear();

		// 封装用户数据
		Message message = { 1, "", "", age, gender };
		strncpy(message.name, name, sizeof(message.name));

		// 发送用户数据
		int r = -1;
#ifdef _WIN32
		r = send(sockfd, (char *)&message, sizeof message, 0);
#else
		r = write(sockfd, &message, sizeof message);
#endif // _WIN32
		if (r == -1)
		{
			std::cout << "Connection Closed" << std::endl;
		}
		// 更新时间
		std::unique_lock<std::mutex> lock(heartbeat_time_mutex); // 对last_heartbeat_time枷锁
		last_heartbeat_time = time(NULL);
		lock.unlock();
	}
}

// 子线程2负责读取来自服务器的数据
void ReadData(int sockfd)
{
	while (true)
	{
		// 接收响应数据，有两类数据，心跳数据和用户数据
		Message response;
		int n = -1;
#ifdef _WIN32
		n = recv(sockfd, (char *)&response, sizeof response, 0);
#else 
		n = read(sockfd, &response, sizeof response);
#endif
		
		// 更新时间
		std::unique_lock<std::mutex> lock2(heartbeat_time_mutex); // 对last_heartbeat_time枷锁
		last_heartbeat_time = time(NULL);
		lock2.unlock();

		if (n <= 0)
		{
			perror("read: ");
			break;
		}

		// 输出响应数据
		if (response.type == 0)
		{
			// 打开一个文件，用于存放收到的心跳数据
#ifdef _WIN32
			FILE * fp = fopen("C:/Users/Administrator/Desktop/projects/ConsoleApplication/x64/Debug/log.txt", "a+");
			
			fprintf(fp, "%s", "RECV A Heart Beat ACK\r\n");

			fclose(fp);
#else
			int fd_target = open("log.txt", O_WRONLY | O_APPEND);
			if (fd_target == -1)
			{
				perror("open");
				break;
			}

			// 计算字节数，适用于中文和英文数字等。
			const char *app_context = "Recv a Heart Beat Data ACK\n";
			write(fd_target, app_context, strlen(app_context) * sizeof(char));
			close(fd_target);
#endif
			
		}
		else if (response.type == 1)
		{
			std::cout << "收到服务器响应的用户数据 --> " << response.name << '\t' << response.age << '\t' << response.gender << std::endl;
		}
	}
}

/**
 * @brief
 *  客户端主线程负责初始化环境，创建两个子线程，然后负责用户的发送的数据输入
 *  子线程1：负责发送定时发送心跳数据维持连接
 *  子线程2：负责读取来自服务端的数据，根据数据类型处理
 * @return int
 */
int main()
{
#ifdef _WIN32
	//初始化网络环境
	//初始化网络环境
	WSAData data;
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret) {
		std::cout << "初始化网络错误！" << std::endl;
		WSACleanup();
		return -1;
	}
#endif
	// 连接到服务器
	int sockfd = connectServer("127.0.0.1", 8888);

	// 启动心跳包线程
	thread heart_thread(sendHeartBeat, sockfd);

	// 开启线程读取数据
	thread read_thread(ReadData, sockfd);

	// 发送和接受用户数据
	sendData(sockfd);

	// 等待心跳包线程结束
	heart_thread.join();
	read_thread.join();

	// 关闭套接字
#ifdef _WIN32
	closesocket(sockfd);
#else
	close(sockfd);
#endif
	
	return 0;
}
```

```c++
/*
	服务端
*/
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
//Windows
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")
#else
//Linux
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif


using namespace std;

// 定义消息类型（心跳数据 + 用户数据）
struct Message
{
	int type;          // 消息类型。0心跳包，1用户数据
	char heartData[8]; // 心跳包数据
	char name[36];     // 姓名
	int age;           // 年龄
	char gender;       // 性别
};

// 处理客户端连接的函数
void handle_client(int client_fd)
{
#ifdef _WIN32
	// 记录最后一次连接活跃的时间，只要是用户数据和心跳包发过来都可以更新这个值
	time_t last_active_time = time(NULL);

	// 设置通信描述符非阻塞
	u_long nonBlocking = 1;
	int ret = ioctlsocket(client_fd, FIONBIO, &nonBlocking);
	if (ret != NO_ERROR) {
		cerr << " Error setting socket to noblock " << endl;
	}

	while (true)
	{
		// 检查客户端是否长时间未发送数据或者心跳数据，如果是，则关闭连接
		if (time(NULL) - last_active_time > 20)
		{
			cout << "Client timeout, closing connection..." << endl;
			closesocket(client_fd);
			break;
		}

		// 接收客户端信息
		Message message;
		int n = recv(client_fd, (char *)&message, sizeof message, 0);
		if (n == 0)
		{
			// 客户端断开连接
			cout << "Client disconnected..." << endl;
			closesocket(client_fd);
			break;
		}
		else if (n == SOCKET_ERROR)  // -1
		{
			continue;		//非阻塞进入
		}
		else if (n > 0)
		{
			if (message.type == 0)
			{
				// 心跳包
				cout << message.type << '\t' << message.heartData << endl;
			}
			else if (message.type == 1)
			{
				// 用户数据
				cout << message.name << '\t' << message.age << '\t' << message.gender << endl;
			}
			else
			{
				//表示不是message的协议方式
				char * pmess = (char *)&message;
				cout << pmess << endl;
			}
		}
		
		// 更新活跃时间
		last_active_time = time(NULL);

		if (message.type == 0)
		{
			// 心跳包
			Message response = { 0, "ACK", "", 0, '\0' };
			send(client_fd, (char *)&response, sizeof response, 0);
		}
		else if (message.type == 1)
		{
			// 用户数据
			message.age = 88;
			send(client_fd, (char *)&message, sizeof message, 0);
		}
	}
#else	//Linux
	// 记录最后一次连接活跃的时间，只要是用户数据和心跳包发过来都可以更新这个值
	time_t last_active_time = time(NULL);

	// 设置通信文件描述符非阻塞
	fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) | O_NONBLOCK);

	while (true)
	{
		// 检查客户端是否长时间未发送数据或者心跳数据，如果是，则关闭连接
		if (time(NULL) - last_active_time > 20)
		{
			cout << "Client timeout, closing connection..." << endl;
			close(client_fd);
			break;
		}

		// 接收客户端信息
		Message message;
		int n = read(client_fd, &message, sizeof message);
		if (n == 0)
		{
			// 客户端断开连接
			cout << "Client disconnected..." << endl;
			close(client_fd);
			break;
		}
		else if (n == -1)
		{
			// 非阻塞进入
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				// cout << "noblock" << endl;
				continue;
			}
		}
		else if (n > 0)
		{ // 读取到了数据
			if (message.type == 0)
			{
				// 心跳包
				cout << message.type << '\t' << message.heartData << endl;
			}
			else if (message.type == 1)
			{
				// 用户数据
				cout << message.name << '\t' << message.age << '\t' << message.gender << endl;
			}
		}
		else
		{
			cout << "Error: " << n << endl;
		}

		// 更新活跃时间
		last_active_time = time(NULL);

		if (message.type == 0)
		{
			// 心跳包
			Message response = { 0, "ACK", "", 0, '\0' };
			write(client_fd, &response, sizeof response);
		}
		else if (message.type == 1)
		{
			// 用户数据
			message.age = 88;
			write(client_fd, &message, sizeof message);
		}
	}
#endif
	
}

int main()
{
#ifdef _WIN32
	//初始化网络环境
	WSAData data;
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret) {
		cout << "初始化网络错误！" << endl;
		WSACleanup();
		return -1;
	}

	// 1、创建socket，返回文件描述符，是一个用于监听的套接字
	SOCKET sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		perror("socket");
		exit(-1); // 失败直接结束进程
	}
	// 2、绑定ip和端口等数据（服务器端自身的特征数据）
	struct sockaddr_in addr; // 先使用专用网络地址封装然后强转即可
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888); // 需要将主机字节    序转换为网络字节序
	// inet_pton(AF_INET, "192.168.47.131", &(addr.sin_addr.s_addr)); //网络中需要将 点分十进制的地址转换为网络中用的整数
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	// 一个电脑可能有多个网卡，如果使用这个宏，那么客户端连接任意一个网卡的ip地址都可以链接到服务端。这个宏相当于0
	ret = ::bind(sfd, (struct sockaddr *)&addr, sizeof(addr));
	// 第二个参数需强转成通用结构
	if (ret == -1)
	{
		perror("bind");
		exit(-1);
	}
	// 3、监听，处于等待客户端连接的阻塞状态
	ret = listen(sfd, 6);
	if (ret == -1)
	{
		perror("listen");
		exit(-1);
	}

	while (true)
	{
		// 等待客户端连接并且开启数据接收和响应
		struct sockaddr_in client_addr;
		int len = sizeof(client_addr);
		SOCKET client_fd = accept(sfd, (struct sockaddr *)&client_addr, &len);

		// 4.1 输出客户端的信息 ip和port （此时又需要将网络字节序转换为主机字节序）
		//string ip = client_addr.sin_addr.S_un.S_addr;
		//inet_ntop(AF_INET, &(client_addr.sin_addr.s_addr), client_ip, 16);
		// 4.2 输出客户端的信息 ip和port （此时又需要将网络字节序转换为主机字节序）
		//cout << "Client IP: " << client_ip << "\tPort: " << ntohs(client_addr.sin_port) << endl;

		cout << "Accept a Client, IP And Port : " << client_addr.sin_addr.S_un.S_addr << " : " << client_addr.sin_port << endl;

		// 开启线程
		thread service(handle_client, client_fd);
		service.detach();
	}

	// 关闭监听套接字
	closesocket(sfd);
#else
	// 1、创建socket，返回文件描述符，是一个用于监听的套接字
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		perror("socket");
		exit(-1); // 失败直接结束进程
	}
	// 2、绑定ip和端口等数据（服务器端自身的特征数据）
	struct sockaddr_in addr; // 先使用专用网络地址封装然后强转即可
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888); // 需要将主机字节    序转换为网络字节序
	// inet_pton(AF_INET, "192.168.47.131", &(addr.sin_addr.s_addr)); //网络中需要将 点分十进制的地址转换为网络中用的整数
	addr.sin_addr.s_addr = INADDR_ANY;
	// 一个电脑可能有多个网卡，如果使用这个宏，那么客户端连接任意一个网卡的ip地址都可以链接到服务端。这个宏相当于0
	int ret = bind(sfd, (struct sockaddr *)&addr, sizeof(addr));
	// 第二个参数需强转成通用结构
	if (ret == -1)
	{
		perror("bind");
		exit(-1);
	}
	// 3、监听，处于等待客户端连接的阻塞状态
	ret = listen(sfd, 6);
	if (ret == -1)
	{
		perror("listen");
		exit(-1);
	}

	while (true)
	{
		// 等待客户端连接并且开启数据接收和响应
		struct sockaddr_in client_addr;
		socklen_t len = (socklen_t)sizeof(client_addr);
		int client_fd = accept(sfd, (struct sockaddr *)&client_addr, &len);
		// 4.1 输出客户端的信息 ip和port （此时又需要将网络字节序转换为主机字节序）
		char client_ip[16];
		inet_ntop(AF_INET, &(client_addr.sin_addr.s_addr), client_ip, 16);
		// 4.2 输出客户端的信息 ip和port （此时又需要将网络字节序转换为主机字节序）
		cout << "Client IP: " << client_ip << "\tPort: " << ntohs(client_addr.sin_port) << endl;

		// 开启线程
		thread service(handle_client, client_fd);
		service.detach();
	}

	// 关闭监听套接字
	close(sfd);
#endif
	
	return 0;
}
```



## 2、select高性能IO

多路IO复用：select函数

```
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

函数作用：委托内核监控可读、可写和异常事件。

函数参数：
	nfds --> 需要监听的最大文件描述符值 + 1
	readfds --> 读集合，存放需要监听的可读事件的文件描述符集合
	writefds --> 写集合，存放需要监听的可写事件的文件描述符集合
	exceptfds --> 异常集合，存放需要监听的异常事件的文件描述符集合
	timeout --> 超时事件，如果在指定时间内没有文件描述符就绪，则select返回0，如果timeout为NULL，则select会一直等待，直达有文件描述符就绪才返回。
返回值说明：
	成功，返回就绪文件描述符的数量。
	失败，返回-1.
```

```
关于fd_set类型相关的操作函数：
void FD_CLR(int fd, fd_set *set);	//将fd再set集合中清除
int FD_ISSET(int fd, fd_set *set);	//判断fd是否在set集合中
void FD_SET（int fd, fd_set * set);	//将fd添加到set集合中
void FD_ZERO(fd_set *set);			//将set全部置为0
```

```
开发流程
获取监听描述符 --> 定义文件描述符集变量 --> 死循环
										select监听
										判断监听文件描述符是否有新的连接请求
										循环判断其他的文件描述符
```



```c++
/*
	服务端Windows版本
*/
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
//Windows
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32")
#else
//Linux
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace std;

// 定义消息类型（心跳数据 + 用户数据）
struct Message
{
	int type;          // 消息类型。0心跳包，1用户数据
	char heartData[8]; // 心跳包数据
	char name[36];     // 姓名
	int age;           // 年龄
	char gender;       // 性别
};

//全局变量，存放时间
const int MAX_FDS = 1024;
time_t IntervalTime[MAX_FDS] = { 0 };

// 互斥锁，保护全局变量
std::mutex Interval_time_mutex;

// 定义文件描述符集变量
fd_set TempFDS, ReadFDS;

// 处理客户端连接的函数
void handle_client(int client_fd)
{
#ifdef _WIN32
	// 记录最后一次连接活跃的时间，只要是用户数据和心跳包发过来都可以更新这个值
	//time_t last_active_time = time(NULL);
	int index = client_fd % MAX_FDS;

	// 对Interval_time_mutex加锁
	std::unique_lock<std::mutex> lock(Interval_time_mutex);
	IntervalTime[index] = time(NULL);	//重置该位置的时间
	//解锁
	lock.unlock();

	// 接收客户端信息
	Message message;
	int n = recv(client_fd, (char *)&message, sizeof message, 0);
	if (n < 0)
	{
		// 客户端断开连接
		cout << "Client disconnected..." << endl;
		closesocket(client_fd);
		// 对Interval_time_mutex加锁
		std::unique_lock<std::mutex> lock(Interval_time_mutex);
		IntervalTime[index] = 0;	//重置该位置的时间
		FD_CLR(client_fd, &ReadFDS);
		//解锁
		lock.unlock();
		//break;
		return;
	}
	else if (n > 0)
	{
		if (message.type == 0)
		{
			// 心跳包
			cout << message.type << '\t' << message.heartData << endl;
		}
		else if (message.type == 1)
		{
			// 用户数据
			cout << message.name << '\t' << message.age << '\t' << message.gender << endl;
		}
		else
		{
			//表示不是message的协议方式
			char * pmess = (char *)&message;
			cout << pmess << endl;
		}
	}
	else
	{
		//break;
		return;
	}

	// 更新活跃时间
	// 对Interval_time_mutex加锁
	std::unique_lock<std::mutex> lock2(Interval_time_mutex);
	IntervalTime[index] = time(NULL);	//重置该位置的时间
	//解锁
	lock2.unlock();

	if (message.type == 0)
	{
		// 心跳包
		Message response = { 0, "ACK", "", 0, '\0' };
		send(client_fd, (char *)&response, sizeof response, 0);
	}
	else if (message.type == 1)
	{
		// 用户数据
		message.age = 88;
		send(client_fd, (char *)&message, sizeof message, 0);
	}

#else	//Linux
	
#endif

}

int main()
{
#ifdef _WIN32
	//初始化网络环境
	WSAData data;
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret) {
		cout << "初始化网络错误！" << endl;
		WSACleanup();
		return -1;
	}

	// 1、创建socket，返回文件描述符，是一个用于监听的套接字
	SOCKET sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		perror("socket");
		exit(-1); // 失败直接结束进程
	}
	// 2、绑定ip和端口等数据（服务器端自身的特征数据）
	struct sockaddr_in addr; // 先使用专用网络地址封装然后强转即可
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888); // 需要将主机字节    序转换为网络字节序

	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	// 一个电脑可能有多个网卡，如果使用这个宏，那么客户端连接任意一个网卡的ip地址都可以链接到服务端。这个宏相当于0
	ret = ::bind(sfd, (struct sockaddr *)&addr, sizeof(addr));
	// 第二个参数需强转成通用结构
	if (ret == -1)
	{
		perror("bind");
		exit(-1);
	}
	// 3、监听，处于等待客户端连接的阻塞状态
	ret = listen(sfd, 6);
	if (ret == -1)
	{
		perror("listen");
		exit(-1);
	}

	// 定义文件描述符集变量
	FD_ZERO(&ReadFDS);	//初始化，清空为0
	FD_SET(sfd, &ReadFDS);	//将监听描述符添加到读的描述符集中
	int maxfd = sfd + 1;
	int nready = 0;
	int cfd = -1;

	while (true)
	{
		TempFDS = ReadFDS;
		//获取有多少个文件描述符有读事件，如果有则说明有客户端发来了数据
		nready = select(maxfd, &TempFDS, NULL, NULL, NULL);
		if (nready < 0)
		{
			if (errno == EINTR) 
			{
				continue;
			}
			break;
		}

		//有客户端连接请求到来。判断sfd是否在tempfds中，如果存在返回true，说明有连接请求
		if (FD_ISSET(sfd, &TempFDS))
		{
			//接收来自新的客户端的连接请求
			cfd = accept(sfd, NULL, NULL);
			//然后将cfd添加到readfds集合中，等待监听其读事件
			FD_SET(cfd, &ReadFDS);
			//修改内核监控的文件描述符的范围
			maxfd = maxfd > cfd ? maxfd : cfd;

			// 初始化时间
			int index = cfd % MAX_FDS;
			// 对Interval_time_mutex加锁
			std::unique_lock<std::mutex> lock(Interval_time_mutex);
			IntervalTime[index] = time(NULL);	//重置该位置的时间
			//解锁
			lock.unlock();

			if (--nready == 0) {
				continue;	
			}
		}

		//到这里表示通信数据交互
		for (int i = sfd + 1; i <= maxfd; i++)
		{
			if (FD_ISSET(i, &TempFDS))
			{
				// 检查客户端是否长时间未发送数据或者心跳数据，如果是，则关闭连接
				int index = i % MAX_FDS;
				if (time(NULL) - IntervalTime[index] > 20)
				{
					cout << "Client timeout, closing connection..." << endl;
					closesocket(i);
					// 对Interval_time_mutex加锁
					std::unique_lock<std::mutex> lock(Interval_time_mutex);
					IntervalTime[index] = 0;	//重置该位置的时间
					//解锁
					lock.unlock();
					//这个位置的通信描述符删除，不要再监听
					FD_CLR(i, &ReadFDS);
				}
				else
				{
					//读取数据并且应答数据
					// 开启线程
					thread service(handle_client, i);
					service.detach();
				}
			}
		}
	}
	
	// 关闭监听套接字
	closesocket(sfd);
#else
	
#endif

	return 0;
}
```



## 3、poll高性能IO

```
int poll(struct pollfd * fds, nfds_t nfds, int timeout);

函数作用：与select函数类似，委托内核监控可读，可写和异常事件。
参数说明：
	- fds：是一个struct pollfd结构体数组，这是一个需要检测的文件描述符的集合（传入传出参数）
		struct pollfd {
			int fd;			//委托内核检测的文件描述符，如果等于-1表示内核不再监控其指定的事件
			short events; 	//委托内核检测文件描述符的是什么事件
			short revents; 	//文件描述符实际发生的事件，由内核赋值返回
		};
		events/revents:
			POLLIN 	- 可读事件
			POLLOUT - 可写事件
			POLLERR - 异常事件
     - nfds：这是第一个参数数组中最后一个有效元素的下标 + 1（即数组中元素个数）
     - timeout: 阻塞时长
     		0 	- 不阻塞
     		-1 	- 阻塞，当检测到需要检测的文件描述符有变化，解除阻塞
     		> 0 - 阻塞时长，在时长范围内有事件发生会立即返回，否则，超过时长返回
返回值：
	-1：失败
	=0：文件描述符没有发生变化
	>0：成功，n表示检测到集合中有几个文件描述符发生变化
```

```c++
/*
	服务端
*/
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
//Windows
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32")
#else
//Linux
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace std;

// 定义消息类型（心跳数据 + 用户数据）
struct Message
{
	int type;          // 消息类型。0心跳包，1用户数据
	char heartData[8]; // 心跳包数据
	char name[36];     // 姓名
	int age;           // 年龄
	char gender;       // 性别
};

//全局变量，存放时间
const int MAX_FDS = 1024;
time_t IntervalTime[MAX_FDS] = { 0 };

// 互斥锁，保护全局变量
std::mutex Interval_time_mutex;

// 定义文件描述符集变量
fd_set TempFDS, ReadFDS;

// 处理客户端连接的函数
void handle_client(int client_fd)
{
#ifdef _WIN32
	
#else	//Linux

#endif

}

int main()
{
#ifndef _WIN32
	//初始化网络环境
	WSAData data;
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret) {
		cout << "初始化网络错误！" << endl;
		WSACleanup();
		return -1;
	}

	// 1、创建socket，返回文件描述符，是一个用于监听的套接字
	SOCKET sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		perror("socket");
		exit(-1); // 失败直接结束进程
	}
	// 2、绑定ip和端口等数据（服务器端自身的特征数据）
	struct sockaddr_in addr; // 先使用专用网络地址封装然后强转即可
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888); // 需要将主机字节    序转换为网络字节序

	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	// 一个电脑可能有多个网卡，如果使用这个宏，那么客户端连接任意一个网卡的ip地址都可以链接到服务端。这个宏相当于0
	ret = ::bind(sfd, (struct sockaddr *)&addr, sizeof(addr));
	// 第二个参数需强转成通用结构
	if (ret == -1)
	{
		perror("bind");
		exit(-1);
	}
	// 3、监听，处于等待客户端连接的阻塞状态
	ret = listen(sfd, 6);
	if (ret == -1)
	{
		perror("listen");
		exit(-1);
	}

	//定义pollfd集合
	struct pollfd client[1024];

	//将监听描述符放在第一个位置，且监听其读事件
	client[0].fd = sfd;
	client[0].events = POLLIN;

	int maxi = 0;
	int nready = 0;
	for (int i = 1; i < 1024; i++)
	{//-1表示不监听
		client[i].fd = -1;
	}

	while (true)
	{
		nready = poll(client, maxi + 1, -1);
		if (nready < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			break;
		}

		//有客户端连接请求
		if (client[0].revents == POLLIN)
		{
			//接收新的客户端连接
			int cfd = accept(sfd, NULL, NULL);
			//寻找client数组中一个可用的位置
			int k = 1;
			for (; k < 1024; k++)
			{
				if (client[k].fd == 1) {
					client[k].fd = cfd;
					client[k].events = POLLIN;
					break;
				}
			}
			//客户端连接数达到最大
			if (k == 1024) {
				close(cfd);
			}
			//修改最大索引
			if (maxi < k) {
				maxi = k;
			}
			if (--nready == 0) {
				continue;
			}
		}

		//有客户端发送过来数据
		for (int j = 1; j <= maxi; j++) {
			int sockfd = j;
			//如果client数组中的fd为-1，表示不再监控
			if (client[j].fd == -1) {
				continue;
			}
			if (client[j].revents == POLLIN) {
				Message message;
				int n = read(sockfd, &message, sizeof Message);
				if (n < 0) {
					close(sockfd);
					client[j].fd = -1;	//重置
				} else
				{
					//发送数据给客户端
					write(sockfd, message, n);
				}
			}
		}
	}

	// 关闭监听套接字
	close(sfd);
#else

#endif

	return 0;
}
```



## 4、epoll高性能IO

### 4.1 说明

epoll模型底层使用红黑树实现，它的查找性能较为稳定。

```c++
API介绍
int epoll_create(int size);
作用：用于创建epoll红黑树的根节点。
参数：
	size：告诉监听文件描述符的数量，在Linux2.6.8之后被忽略了，传入一个大于0的数即可。
返回值：
	正常情况下：非负整数，即一个文件描述符（epoll红黑树根节点）。
	出现错误返回-1

int epoll_ctl(int epfd, int op, int fd, struct epoll_event * event);
作用：向epoll树中添加、删除或者修改文件描述符及其事件。是一个对epoll进行控制操作的函数。
返回值：成功0；失败-1
参数：
	epfd：epoll树的根节点。即epoll_create函数返回的值。
	op：操作的类型，可以是：EPOLL_CTL_ADD - 添加节点
					    EPOLL_CTL_MOD - 修改节点
                          EPOLL_CTL_DEL - 删除节点
    fd：需要进行操作的文件描述符，即被操作的对象
    event：结构体类型的事件对象，用于描述文件描述符的事件类型。
    		这个对象主要是确定events的值，然后是data的fd字段的值（=fd参数）
        struct epoll_event {
        	//监听的事件。EPOLLIN(输入事件)、EPOLLOUT(输出事件)、EPOLLERR(异常错误)、EPOLLET(ET边沿模式)
        	//多个事件组合用 | 按位或运算符连接
            uint32_t events;
            
            //表示用户自定义的数据，即epoll_ctl函数注册时传递的参数，可以是一个指针也可以是一个文件描述符等等。
            epoll_data_t data;
        };
        typedef union epoll_data {	//联合体
        	void * ptr;
        	int fd;			//文件描述符，通常使用这个
        	unint 32_t u32;
        	uint64 u_64;        
        } epoll_data_t;
        
int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
作用：用于等待文件描述符的事件就绪（检测是否有epoll注册之后的节点的事件触发）
返回值：发生事件的文件描述符的数量。如果设置了超时参数，那么超时就返回0；函数出错返回-1
参数：
    epfd：epoll树的根节点
    events：一个指向epoll_event数组的指针，用于存放已经就绪的事件
    maxevents：events数组的大小
    timeout：超时时间，单位是毫秒；设置为-1，表示阻塞，直到有文件描述符发生事件。
    							设置为0，不阻塞，继续下一行代码。
    							大于0，阻塞的时长。
```

### 4.2 基本流程代码

```cpp
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

int main()
{
    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    // 绑定
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    // 监听
    listen(lfd, 8);
    
    //1、创建epoll树的根节点，负责整个epoll的管理
    int epfd = epoll_create(100);
    //2、将监听文件描述符添加到epoll树中，作为一个子节点，监听其EPOLLIN事件，一旦触发说明存在来自客户端的连接，需要accept操作
    struct epoll_event epv;
    epv.events = EPOLLIN;
    epv.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epv);
    
    //负责存放被触发的事件节点信息
    struct epoll_event epvs[1024] = {
        .events = -1,
        .data.fd = -1;
    };
    
    while(true) {
        //3、监听事件响应
        int nready = epoll_wait(epfd, epvs, 1024, -1);
        if(nready == -1) { perror("epoll_wait"); return; }
        
        //遍历所产生的各个文件描述符的事件
        for(int i = 0; i < nready; i++) {
            //如果是监听文件描述符，就说明有客户端请求连接
            int curfd = epevs[i].data.fd;
            if (curfd == lfd)
            {
                // 监听的文件描述符有数据达到，有客户端连接
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
                
                //4、将获取的客户端请求连接的文件描述符添加到epoll树中，监听客户端的数据交互
                epv.events = EPOLLIN;
                epv.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epv);
            }
            else
            {
                if (epevs[i].events & EPOLLOUT)
                {
                    continue;
                }
                // 有数据到达，需要通信
                char buf[1024] = {0}; // 该数组用于接收数据
                int len = read(curfd, buf, sizeof(buf));
                if (len == -1)
                {
                    perror("read");
                    //读取时发生错误，那么就不再监听的这个文件描述符，断开。将其从epoll树上剔除
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                }
                else if (len == 0)
                {
                    printf("client closed...\n");
                    //客户端与服务端的连接关闭，那么就不再监听这个文件描述符，将其从epoll树上剔除
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                }
                else if (len > 0)
                {
                    printf("read buf = %s\n", buf);
                    write(curfd, buf, strlen(buf) + 1);
                }
            }
        }
    }
    close(lfd);
    close(epfd);
    return 0;
}

```

### 4.3 epoll内部结构图（反应堆）

![epoll反应堆思想](04-epoll模型\epoll反应堆思想.png)

### 4.4 epoll的事件模式

```
EPOLL事件模式有两种模型：
	Edge-Triggered(ET) 边缘触发，只有数据到来才触发，不管缓存区中是否还有数据。
					可以理解为要是一次性读不完缓存区的数据，就不会再读取。而是等到下一次触发事件之后继续读取缓存区的内容。
     Level-Triggered(LT) 水平触发，只有缓存区中有数据就会一直触发事件。这是默认的事件模式。
     
     假设通信协议有2KB的大小，客户端每次发送都是2KB，如果服务端一次读取1KB的数据，那么就还有1KB的还停留在缓冲区，
   		此时有上面两种模式：
   				默认情况下，会再次触发，epoll_wait函数会监听到，再次读取。（LT模式）
   				ET边缘触发模式下，读取了1KB就不再读了，而是等到客户端再次主动触发事件才会被触发。
                
    LT水平触发模式同时支持阻塞和非阻塞的socket。在这种做法中，内核会告诉你一个文件描述符是否就绪，然后就可以对这个就绪的fd进行IO操作。如果不做任何操作，内核还会一直通知。传统的select/poll都是这种模式的代表。
    ET边沿触发模式有着告诉的工作方式，只支持非阻塞的socket。在这种模式下，当描述符从未就绪变为就绪时，内核通过epoll告诉你。然后它会假设你知道文件描述符已经就绪，并且不会再为那个文件描述符发送更多的就绪通知。请注意，如果一直不对这个fd作IO操作(从而导致它再次变成未就绪)，内核不会发送更多的通知(only once).
```

```c++
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
int main()
{
    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    // 绑定
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    // 监听
    listen(lfd, 8);
    // 调用epoll_create()创建一个epoll实例
    int epfd = epoll_create(100);
    // 将监听的文件描述符相关的检测信息添加到epoll实例中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);
    struct epoll_event epevs[1024];
    while (1)
    {
        int ret = epoll_wait(epfd, epevs, 1024, -1);
        if (ret == -1)
        {
            perror("epoll_wait");
            exit(-1);
        }
        printf("ret = %d\n", ret);
        for (int i = 0; i < ret; i++)
        {
            int curfd = epevs[i].data.fd;
            if (curfd == lfd)
            {
                // 监听的文件描述符有数据达到，有客户端连接
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
                // 设置cfd属性非阻塞。和客户端通信的文件描述要设置为非阻塞，因为使用了边沿触发的模式。
                // 后面要通过循环才能完全读取客户端一次发过来的数据，为了防止读完数据后，read一直处于阻塞状态，导致无法继续监测文件描述符的问题。
                int flag = fcntl(cfd, F_GETFL);
                flag | O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);	//设置非阻塞
                epev.events = EPOLLIN | EPOLLET; // 设置边沿触发（需要主动开启该模式）
                epev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);
            }
            else
            {
                if (epevs[i].events & EPOLLOUT)
                {
                    continue;
                }
                /*
                边沿触发模式下，只有在客户端发送了数据epoll_wait才会被触发，
                如果下面的buf容量太小，一次性无法读取客户端发送过来的数据，那么只能截取
                到其中一部分，即使缓冲区中还有数据没有读完，下次循环也不会触发epoll_wait，
                只有当用户端再次发送数据时，服务端才能被触发读取到缓冲区的数据，不是新发
                送的数据，而是接着上次读到位置继续读取缓冲区中的数据。
                所以，就不得不使用循环来解决一次性无法读取所有数据的问题，不然数据会不完整，甚至出现接收到两次客户端发来的数据。
                */
                // 循环读取出所有数据
                char buf[5];
                int len = 0;
                while ((len = read(curfd, buf, sizeof(buf))) > 0)
                {
                    // 打印数据
                    // printf("recv data : %s\n", buf);
                    write(STDOUT_FILENO, buf, len);
                    write(curfd, buf, len);
                }
                if (len == 0)
                {
                    printf("client closed....");
                }
                else if (len == -1)
                {	//非阻塞情况下，读完之后返回-1
                    if (errno == EAGAIN)
                    {
                        // EAGAIN --> the file descriptor fd refers to a file other than a socket and has been marked non-blocking, and the read would block.
                        printf("data over.....");
                    }
                    else
                    {
                        perror("read");
                        exit(-1);
                    }
                }
            }
        }
    }
    close(lfd);
    close(epfd);
    return 0;
}
```

