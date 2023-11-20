/*
	客户端代码
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