#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#include <string>
#include <event2/thread.h>
#include <thread>
#ifndef _WIN32
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#endif // !_WIN32

using namespace std;

#ifndef _WIN32

void ReadLog(evutil_socket_t fd, short what, void *arg)
{
	char buf[1024] = { 0 };
	int n = read(fd, buf, sizeof buf);
	if (n > 0)
	{
		cout << string(buf, 0, n) << endl;
	}
	else
	{
		cout << "." << flush;
		this_thread::sleep_for(500ms);
	}
}
#endif // !_WIN32

int main()
{
#ifdef _WIN32
	// Windows平台下需要初始化socket库
	WSADATA wdata;
	WSAStartup(MAKEWORD(2, 2), &wdata);
#else
	// linux平台下忽略管道信号，发送数据给已经关闭的socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;

	// 初始化配置
	event_config *config = event_config_new();
	// 设置监听文件描述符
	event_config_require_features(config, EV_FEATURE_FDS);
	// 创建上下文对象
	event_base *base = event_base_new_with_config(config);
	if (base == NULL)
	{
		cerr << "event_base 创建上下文对象错误..." << endl;
	}
	// 释放配置对象
	event_config_free(config);

	// poll模型，因为设置了EV_FEATURE_FDS特征，epoll模型是不支持的，所以根据优先级选择了poll模型
	cout << event_base_get_method(base) << endl;

	// 打开一个linux的用户登录日志文件, 读取文件且非阻塞读
	int fd = open("/var/log/auth.log", O_RDONLY | O_NONBLOCK);

	// 偏移到文件末尾，只监测新写入的日志
	lseek(fd, 0, SEEK_END);

	// 创建事件
	event *revent = event_new(
		base,
		fd,
		EV_READ | EV_PERSIST,
		ReadLog,
		0);

	// 添加事件
	event_add(revent, NULL);

	// 事件分发处理
	event_base_dispatch(base);

	// 释放
	if (revent)
		event_free(revent);
	if (base)
		event_base_free(base);

#endif
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}