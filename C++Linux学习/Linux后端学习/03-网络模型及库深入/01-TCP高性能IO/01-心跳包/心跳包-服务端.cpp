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