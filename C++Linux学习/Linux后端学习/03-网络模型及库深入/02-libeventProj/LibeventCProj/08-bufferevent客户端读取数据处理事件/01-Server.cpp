#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#else
#endif
#include <string>
using namespace std;

//将读取到的数据保存下来
static string recvData = "";

//事件回调
void event_cb(bufferevent *be, short events, void *arg)
{
	cout << "client Event CB" << endl;

	//读取超时事件发生后读取数据停止
	if (events && BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
	{
		//读取缓冲中的数据
		char data[1024] = { 0 };
		//读取网络缓冲区中的数据，因为设置了水位，可能会有数据残留在缓冲中
		int len = bufferevent_read(be, data, sizeof(data) - 1);
		if (len > 0)
		{
			recvData += data;
		}

		cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << endl;
		//释放事件
		bufferevent_free(be);
		//输出接收到的数据，一次性输出
		cout << recvData << endl;
	}
	else if (events & BEV_EVENT_ERROR)
	{
		bufferevent_free(be);
	}
	else
	{
		cout << "others" << endl;
	}
}

//写回调
void write_cb(bufferevent *be, void *arg)
{
	cout << "Write CB" << endl;
}

//读回调
void read_cb(bufferevent *be, void *arg)
{
	cout << "Read CB" << endl;

	char data[1024] = { 0 };
	//读取网络输入缓冲区中的数据
	int len = bufferevent_read(be, data, sizeof(data) - 1);

	if (len <= 0) return;

	//保存数据
	recvData += data;

	//发送数据，写入到网络输出缓冲区
	bufferevent_write(be, "OK", 3);
}

//连接回调触发
void listen_cb(struct evconnlistener *, evutil_socket_t fd, struct sockaddr * addr, int socklen, void * arg)
{
	cout << "listen CB" << endl;

	//上下文对象
	event_base * base = (event_base *)arg;

	//创建bufferevent上下文对象 BEV_OPT_CLOSE_ON_FREE清理bufferevent时关闭socket
	bufferevent * bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	//添加监控事件
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	//设置水位
	//读取水位
	bufferevent_setwatermark(bev, EV_READ,
		5,	//低水位 0就是无限制 默认是0
		10	//高水位 0就是无限制 默认是0
	);

	bufferevent_setwatermark(bev, EV_WRITE,
		5,	//低水位 0就是无限制 默认是0 缓冲数据低于5 写入回调被调用
		0	//高水位无效
	);

	//设置超时时间
	timeval tv = { 0, 500000 };
	//设置读取的超时时间
	bufferevent_set_timeouts(bev, &tv, 0);

	//设置回调函数
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, base);
}

int main(int argc, char *argv[])
{

#ifdef _WIN32 
	//初始化socket库
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#else
	//忽略管道信号，发送数据给已关闭的socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;
#endif
	//上下文对象
	event_base *base = event_base_new();

	//创建网络服务器
	//设定监听的端口和地址
	sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(5001);

	//连接监听器
	evconnlistener * listener = evconnlistener_new_bind(
		base,
		listen_cb,
		base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,
		(sockaddr *)&saddr,
		sizeof saddr
		);

	//进入事件主循环
	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_base_free(base);

	return 0;
}