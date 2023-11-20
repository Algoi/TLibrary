#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#ifndef _WIN32
#include<signal.h>
#endif // !_WIN32

using namespace std;

#define PORT 8888

void listen_cb(struct evconnlistener * listener, evutil_socket_t fd, struct sockaddr * saddr, int socklen, void *arg)
{
	cout << "listen_cb" << endl;
}

int main()
{
#ifdef _WIN32
	//Windows平台下需要初始化socket库
	WSADATA wdata;
	WSAStartup(MAKEWORD(2, 2), &wdata);
#else
	//linux平台下忽略管道信号，发送数据给已经关闭的socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;
#endif

	cout << "test server..." << endl;
	event_base * base = event_base_new();
	if (base)
	{
		cout << "event_base success..." << endl;
	}
	
	sockaddr_in saddr;
	memset(&saddr, 0x0, sizeof saddr);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);
	//监听链接器：封装了socket，bind，listen以及accept功能
	evconnlistener * listener = evconnlistener_new_bind(
		base,				//libevent的上下文，根节点
		listen_cb, base,	//接收到连接时的回调函数(有新连接就会触发)，及其参数
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, //设置端口复用以及listener关闭时关闭对应的监听socket文件描述符
		10,						//连接队列大小，listen函数的第二个参数
		(sockaddr *)&saddr,		//绑定的地址和端口
		sizeof(saddr)			//saddr的字节大小
		);
	//事件分发处理
	event_base_dispatch(base);

	if (listener)
		evconnlistener_free(listener);
	if(base)
		event_base_free(base);

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}