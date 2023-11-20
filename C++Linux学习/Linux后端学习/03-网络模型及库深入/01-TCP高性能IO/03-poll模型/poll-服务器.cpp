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
	closesocket(sfd);
#else

#endif

	return 0;
}