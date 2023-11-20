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

//错误，超时（连接断开会进入）
void client_event_cb(bufferevent *be, short events, void *arg)
{
	cout << "client Event CB" << endl;

	//读取超时事件发生后，数据读取停止
	if (events & BEV_EVENT_READING && events & BEV_EVENT_READING)
	{
		cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << endl;
		bufferevent_free(be);
		return;
	}
	else if (events & BEV_EVENT_ERROR)
	{
		bufferevent_free(be);
		return;
	}

	//服务端的关闭事件
	if (events & BEV_EVENT_EOF)
	{
		cout << "BEV_EVENT_EOF" << endl;
		bufferevent_free(be);
	}

	//客户端与服务端连接成功
	if (events & BEV_EVENT_CONNECTED)
	{
		cout << "CONNECTED SUCCESSFULL" << endl;
		//让客户端主动触发write事件
		bufferevent_trigger(be, EV_WRITE, 0);
	}
}

//写回调
void client_write_cb(bufferevent *be, void *arg)
{
	cout << "client Write CB" << endl;
	
	//获取文件指针
	FILE * fp = (FILE *)arg;
	char readFile_data[1024] = { 0 };
	//读取文件，返回读取到的长度
	int read_len = fread(readFile_data, 1, sizeof(readFile_data) - 1, fp);
	if (read_len <= 0)
	{
		//读到结尾或者文件错误
		fclose(fp);	//关闭文件描述符

		//如果立刻将be事件清理，可能会造成缓冲数据没有发送完成而结束
		//bufferevent_free(be);

		//将写回调撤销，不再监听be事件的写回调
		bufferevent_disable(be, EV_WRITE);
		return;
	}
	//写入buffer
	bufferevent_write(be, readFile_data, read_len);
}

//读取回调
void client_read_cb(bufferevent *be, void *arg)
{
	cout << "cleint Read CB" << endl;
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
	//创建上下文对象
	event_base * base = event_base_new();

	//创建bufferevent事件, -1表示内部创建socket
	bufferevent * bevent = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

	//创建端口和地址对象，远程服务端的地址和端口
	sockaddr_in saddr;
	memset(&saddr, 0x0, sizeof saddr);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(5001);
	evutil_inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);

	//打开一个文件，获取其指针
	FILE *fp = fopen("info.txt", "rb");

	//设置回调函数
	bufferevent_setcb(bevent, client_read_cb, client_write_cb, client_event_cb, fp);
	bufferevent_enable(bevent, EV_READ | EV_WRITE);

	//连接到客户端
	int ret = bufferevent_socket_connect(bevent, (sockaddr *)&saddr, sizeof saddr);
	if (ret == 0)
	{
		cout << "connected..." << endl;
	}

	//进入事件主循环
	event_base_dispatch(base);
	event_base_free(base);

	return 0;
}