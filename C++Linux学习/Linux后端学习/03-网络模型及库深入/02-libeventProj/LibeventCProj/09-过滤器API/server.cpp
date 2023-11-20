#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <string.h>
#include <event2/event_struct.h>
#include <event2/bufferevent_struct.h>
#include <event2/buffer.h>
#include <ctype.h>
#ifndef _WIN32
#include <signal.h>
#else
#endif
#include <string>
using namespace std;

//事件回调
void event_cb(bufferevent *be, short events, void *arg)
{
	cout << "client Event CB" << endl;
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

	//读取来自过滤的数据
	char data[1024] = { 0 };
	bufferevent_read(be, data, sizeof(data) - 1);
	cout << data << endl;

	//响应数据
	char rdata[1024] = "<h1>Hello World</h1>";
	bufferevent_write(be, rdata, strlen(rdata));
}

//输入过滤器
enum bufferevent_filter_result filter_input_cb(
	struct evbuffer *src, struct evbuffer *dst, ev_ssize_t dst_limit,
	enum bufferevent_flush_mode mode, void *ctx)
{
	cout << "Input CB" << endl;

	char data[1024];
	//从缓冲区中读取数据
	int readLen = evbuffer_remove(src, data, sizeof(data) - 1);

	//将字母转成大写
	for (int i = 0; i < sizeof(data) - 1; i++)
	{
		data[i] = toupper(data[i]);
	}

	//将处理之后的数据写到目标缓冲区，给读事件发送数据
	evbuffer_add(dst, data, readLen);

	return BEV_OK;
}

//输出过滤器
enum bufferevent_filter_result filter_output_cb(
	struct evbuffer *src, struct evbuffer *dst, ev_ssize_t dst_limit,
	enum bufferevent_flush_mode mode, void *ctx)
{
	cout << "Output CB" << endl;

	//过滤输出的数据，将其添加http协议头
	string str = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nConten-Length: ";

	char data[1024] = { 0 };
	int readLen = evbuffer_remove(src, data, sizeof(data) - 1);

	str += std::to_string(readLen);
	str += "\r\n\r\n";
	str += data;

	evbuffer_add(dst, str.data(), str.size());

	return BEV_OK;
}

//连接回调触发
void listen_cb(struct evconnlistener *, evutil_socket_t fd, struct sockaddr * addr, int socklen, void * arg)
{
	cout << "listen CB" << endl;

	//上下文对象
	event_base * base = (event_base *)arg;

	//创建bufferevent上下文对象 BEV_OPT_CLOSE_ON_FREE清理bufferevent时关闭socket
	bufferevent * bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	//创建event过滤器
	bufferevent * bev_filter = bufferevent_filter_new(bev,
		filter_input_cb,		//输入过滤器回调函数
		filter_output_cb,		//输出过滤器回调函数
		BEV_OPT_CLOSE_ON_FREE,	//在释放bev_filter对象的时候释放与之关联的bev对象
		NULL,
		NULL
	);

	//添加监控事件
	bufferevent_enable(bev_filter, EV_READ | EV_WRITE);

	//设置回调函数
	bufferevent_setcb(bev_filter, read_cb, write_cb, event_cb, base);
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