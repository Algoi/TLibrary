#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <iostream>
#include <string>
using namespace std;

#define SPORT 5001

struct Status
{
	bool start = false;
	FILE *fp = 0;
	//string filename;
};

//输入过滤器
bufferevent_filter_result filter_in(evbuffer *s, evbuffer *d,
	ev_ssize_t limit, bufferevent_flush_mode mode, void *arg)
{
	//1 接收客户端发送的文件名
	char data[1024] = { 0 };
	int len = evbuffer_remove(s, data, sizeof(data) - 1);

	//2、继续向读回调函数传数据，把文件名传过去
	evbuffer_add(d, data, len);
	return BEV_OK;
}

//读回调函数
static void read_cb(bufferevent *bev, void *arg)
{
	Status *status = (Status *)arg;
	if (!status->start)
	{
		//001接收文件名
		char data[1024] = { 0 };
		bufferevent_read(bev, data, sizeof(data) - 1);
		//status->filename = data;
		string out = "out\\";
		out += data;
		//打开写入文件
		status->fp = fopen(out.c_str(), "wb");
		if (!status->fp)
		{
			cout << "server open " << out << " failed!" << endl;
			return;
		}

		//002 回复OK
		bufferevent_write(bev, "OK", 2);
		status->start = true;
		return;
	}

	do
	{
		//写入文件
		char data[1024] = { 0 };
		int len = bufferevent_read(bev, data, sizeof(data));
		if (len >= 0)
		{
			fwrite(data, 1, len, status->fp);
			fflush(status->fp);
		}	//只要读缓冲区中的数据长度大于0就一直读
	} while (evbuffer_get_length(bufferevent_get_input(bev)) > 0);
}

//事件回调函数
static void event_cb(bufferevent *bev, short events, void *arg)
{
	cout << "server event_cb " << events << endl;
	Status *status = (Status *)arg;
	if (events & BEV_EVENT_EOF)
	{
		cout << "server event BEV_EVENT_EOF" << endl;
		if (status->fp)
		{
			fclose(status->fp);
			status->fp = 0;
		}
		bufferevent_free(bev);
	}
}

//监听回调函数
static void listen_cb(struct evconnlistener * e, evutil_socket_t s, struct sockaddr *a, int socklen, void *arg)
{
	cout << "listen_cb" << endl;
	event_base *base = (event_base *)arg;
	//1 创建一个bufferevent 用来通信
	bufferevent *bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);
	Status *status = new Status();

	//2 添加过滤 并设置输入回调
	bufferevent *bev_filter = bufferevent_filter_new(bev,
		filter_in,//输入过滤函数
		0,//输出过滤
		BEV_OPT_CLOSE_ON_FREE,//关闭filter同时管理bufferevent
		0, //清理回调
		status	//传递参数
	);

	//3 设置回调 读取 事件（处理连接断开） 
	bufferevent_setcb(bev_filter, read_cb, 0, event_cb, status);
	bufferevent_enable(bev_filter, EV_READ | EV_WRITE);
}

//服务端主程序
void Server(event_base*base)
{
	cout << "begin Server" << endl;
	//监听端口
//socket ，bind，listen 绑定事件
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);

	evconnlistener *ev = evconnlistener_new_bind(base,	// libevent的上下文
		listen_cb,					//接收到连接的回调函数
		base,						//回调函数获取的参数 arg
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,//地址重用，evconnlistener关闭同时关闭socket
		10,							//连接队列大小，对应listen函数
		(sockaddr*)&sin,							//绑定的地址和端口
		sizeof(sin)
	);

}