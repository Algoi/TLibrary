#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#include <event2/thread.h>
#ifndef _WIN32
#include<signal.h>
#endif // !_WIN32

using namespace std;

#define PORT 8888

void listen_cb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *)
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

	//-------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------

	//初始化配置
	event_config * config = event_config_new();

	//1、显示支持的网络模型
	const char ** methods = event_get_supported_methods();
	for (int i = 0; methods[i] != NULL; i++)
	{	//如果是windows系统，输出一个win32
		//如果是linux系统，输出epoll、poll、select三个
		cout << methods[i] << endl;
	}

	//2.1、设置特征
	/**
		event_config_require_features(config, event_method_feature)函数
			EV_FEATURE_ET = 0x01, 边沿触发的后端
			EV_FEATURE_O1 = 0x02, 要求添加、删除单个事件，或者确定哪个事件激活的操作是O（1）复杂度的后端
			EV_FEATURE_FDS = 0x04, 要求支持任意文件描述符，而不仅仅是套接字的后端（一旦打开不能选择epoll模型）
			EV_FEATURE_EARLY_CLOSE = 0x0：
				检测连接关闭事件。您可以使用它来检测连接何时关闭，而不必从连接中读取所有挂起的数据。并非所有后端都支持EV_CLOSED。
				允许您使用EV_CLOSED检测连接关闭，而不需要读取所有挂起的数据。
			//windows系统下，以上四个全部默认不支持；Linux系统下，以上四个只有第三个不支持
	*/
	//设置第三种特征，该特征设置之后，会导致创建上下文base失败。并且加了这个特征会导致其他特征也无法设置。
	//在linux环境下第三种特征可以设置成功。
	//event_config_require_features(config, EV_FEATURE_FDS);
	//event_config_require_features(config, EV_FEATURE_ET);

	//3.1、Linux环境下设置网络模型,使用select。windows环境只有win32
	/*event_config_avoid_method(config, "epoll");
	event_config_avoid_method(config, "poll");*/

	//4.1、windows中支持IOCP设置（线程池）。一旦使用IOCP在启动时就会开启线程池
#ifdef _WIN32
	event_config_set_flag(config, EVENT_BASE_FLAG_STARTUP_IOCP);
	//初始化iocp的线程
	evthread_use_windows_threads();
	//设置cpu数量
	SYSTEM_INFO sinfo;
	GetSystemInfo(&sinfo);	//获取计算机信息
	event_config_set_num_cpus_hint(config, sinfo.dwNumberOfProcessors);
#endif // _WIN32

	//--------------------------------------------------------------------------------------
	//创建上下文，需要通过配置创建
	event_base * base = event_base_new_with_config(config);
	if (base == NULL) {
		//如果通过自定义配置创建失败，那么就通过默认配置创建
		cout << "配置创建上下文失败" << endl;
		base = event_base_new();
		if (base == NULL) {
			return 0;
		}
	}
	event_config_free(config);

	//3.2、获取当前上下文的网络模型
	//windows平台下就是win32；linux平台下是epoll（默认）
	const char * curmethod = event_base_get_method(base);
	cout << curmethod << endl;

	//2.2、确认特征是否生效
	int confrim = event_base_get_features(base);
	if (confrim & EV_FEATURE_ET)
	{
		cout << "EV_FEATURE_ET 边沿触发" << endl;
	}
	else {
		cout << "未设置边沿触发" << endl;
	}
	if (confrim & EV_FEATURE_O1)
	{
		cout << "EV_FEATURE_O1" << endl;
	}
	else {
		cout << "未设置EV_FEATURE_O1" << endl;
	}
	if (confrim & EV_FEATURE_FDS)
	{
		cout << "EV_FEATURE_FDS" << endl;
	}
	else {
		cout << "未设置EV_FEATURE_FDS" << endl;
	}
	if (confrim & EV_FEATURE_EARLY_CLOSE)
	{
		cout << "EV_FEATURE_EARLY_CLOSE" << endl;
	}
	else {
		cout << "未设置EV_FEATURE_EARLY_CLOSE" << endl;
	}

	//-------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------

	sockaddr_in saddr;
	memset(&saddr, 0x0, sizeof saddr);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);

	//监听链接器
	evconnlistener * listener = evconnlistener_new_bind(
		base,				
		listen_cb, base,	
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,						
		(sockaddr *)&saddr,		
		sizeof(saddr)			
	);

	//事件分发处理
	event_base_dispatch(base);

	if (listener)
		evconnlistener_free(listener);
	if (base)
		event_base_free(base);

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}