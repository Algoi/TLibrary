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