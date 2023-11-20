#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <string.h>
#include <stdlib.h>
#ifndef _WIN32
#include <signal.h>
#else
#endif
using namespace std;

//错误，超时 （连接断开会进入）
void event_cb(bufferevent *be, short events, void *arg)
{
	//超时事件发生，数据读取会停止
	if (events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
	{
		cout << "timeout..." << endl;
		//方案1：退出并关闭socket(比如http服务器，超时断开连接)
		bufferevent_free(be);
		//方案2：重新让其可读
		//bufferevent_enable(be, EV_READ);
	}
	else if(events & BEV_EVENT_ERROR)
	{//发生错误事件
		bufferevent_free(be);
	}
	else
	{
		cout << "[E]\n" << flush;
	}
	
}

//写回调
void write_cb(bufferevent *be, void *arg)
{
	cout << "[W]\n" << flush;
}

//读回调
void read_cb(bufferevent *be, void *arg)
{
	cout << "[R]\n" << flush;
	char data[1024] = { 0 };

	//读取输入缓冲数据
	int len = bufferevent_read(be, data, sizeof(data) - 1);
	cout << "[" << data << "]" << endl;
	if (len <= 0) return;
	if (strstr(data, "quit") != NULL)
	{
		cout << "quit";
		//退出并关闭socket BEV_OPT_CLOSE_ON_FREE
		bufferevent_free(be);
	}
	//发送数据 写入到输出缓冲，像浏览器页面发送一个h1标签数据
	string rdata = "HTTP/1.1 200 OK\r\nContent-type:text/html\r\nContent-Length:14\r\n\r\n<h1>HELLO</h1>";
	//bufferevent_write(be, "OK", 3);
	bufferevent_write(be, rdata.data(), rdata.size());
}

//监听回调
void listen_cb(evconnlistener *ev, evutil_socket_t fd, sockaddr*sin, int slen, void *arg)
{
	cout << "-------------- listen_cb --------------" << endl;

	//上下文节点
	event_base *base = (event_base *)arg;

	//创建bfferevent的上下文对象，BEV_OPT_CLOSE_ON_FREE清理bufferevent时关闭socket
	bufferevent * bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	//添加监控事件
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	//1、设置水位
	//读取水位。低水位要求接收到至少5个字节的数据才会被接受
	//高水位设置为10，如果发送的数据超过了10，那么每次读取的数据不会超过10，而是通过多次触发读取回调函数
	bufferevent_setwatermark(bev, EV_READ,
		5,		//低水位，0就是不限制，默认为0
		10		//高水位，0就是不限制，默认为0
	);

	bufferevent_setwatermark(bev, EV_WRITE,
		5,		//低水位，0就是不限制，默认为0。缓冲数据低于5写入回调函数被调用
		0		//高水位无效，不设置即可
	);

	//2、超时时间的设置
	timeval tv = { 3, 0 };
	bufferevent_set_timeouts(bev, &tv, 0);	//设置读取事件超时时间

	//设置回调函数
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, base);
}

int main()
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
	//创建上下文对象
	event_base *base = event_base_new();

	//设定监听的端口和地址
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(5001);

	//连接监听器
	evconnlistener *ev = evconnlistener_new_bind(base,
		listen_cb,		//回调函数
		base,			//回调函数的参数arg
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,				//listen back
		(sockaddr*)&sin,
		sizeof(sin)
	);

	//进入事件主循环
	event_base_dispatch(base);
	evconnlistener_free(ev);
	event_base_free(base);
#ifdef _WIN32
	WSACleanup();
#endif // _WIN32

	return 0;

}

