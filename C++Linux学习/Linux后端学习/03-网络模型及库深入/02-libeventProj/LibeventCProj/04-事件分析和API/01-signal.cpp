#include <iostream>
#include <event2/event.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif // !_WIN32

using namespace std;

#define PORT 8888

void ctrl_c_cb(int sock, short which, void *arg)
{
	cout << "sock: " << sock << endl;
	cout << "which: " << which << endl;
	if (which & EV_TIMEOUT)
	{
		cout << "EV_TIMEOUT" << endl;
	}
	if (which & EV_SIGNAL)
	{
		cout << "EV_SIGNAL" << endl;	//只有这个会输出
	}
	if (which & EV_PERSIST)
	{
		cout << "EV_PERSIST" << endl;
	}
	if (which & EV_ET)
	{
		cout << "EV_ET" << endl;
	}
}

void Kill_cb(int sock, short which, void *arg)
{
	cout << "kill " << endl;
	//如果处于非待决状态
	event * ev = (struct event *)arg;
	if (!evsignal_pending(ev, NULL))
	{
		event_del(ev);
		event_add(ev, NULL);
	}
}

void test()
{
#ifdef _WIN32
#else
	// linux平台下忽略管道信号，发送数据给已经关闭的socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;

	event_base *base = event_base_new();

	/*
		1、使用信号事件对应的API宏
			#define evsignal_add(ev, tv)			event_add((ev), (tv))
			#define evsignal_new(b, x, cb, arg)		event_new((b), (x), EV_SIGNAL|EV_PERSIST, (cb), (arg))
			#define evsignal_del(ev)				event_del(ev)
			#define evsignal_pending(ev, tv)		event_pending((ev), EV_SIGNAL, (tv))
	*/
	// 添加ctrl + c
	event *cevent = evsignal_new(base, SIGINT, ctrl_c_cb, base);
	if (cevent == NULL)
	{
		cout << "evsignal failed..." << endl;
		return -1;
	}
	// 添加事件
	if (evsignal_add(cevent, 0) != 0)
	{
		cout << "event add failed..." << endl;
	}

	/*
		2、使用event_new设置信号事件，添加kill信号

		函数event_self_cbarg()：传递当前的ksigevent对象
	*/
	event * ksigevent = event_new(base, SIGTERM, EV_SIGNAL, Kill_cb, event_self_cbarg());

	event_add(ksigevent, NULL);

	// 主事件循环
	//event_base_dispatch(base);
	//EVLOOP_ONCE 等待一个事件运行，直到没有活动的事件就退出。比如这里的信号事件，当出发了一个信号，执行完回调之后就会结束循环退出程序。
	//EVLOOP_NONBLOCK  有活动事件处理，没有活动事件，立刻返回0（正常退出）。比如这里都是信号事件，启动程序时没有任何事件需要处理，那么就直接退出了程序。可以配合循环使用
	//EVLOOP_NO_EXIT_ON_EMPTY  没有添加事件（event_add）也不返回。场景：可以在多线程中添加事件。
	//event_base_dispatch(base) = event_base_loop(base, 0); 只要有事件添加到了event_base上下文中，那么就一直循环。如果事件没有被添加到base中，那么直接退出循环。
	event_base_loop(BASETYPES, EVLOOP_NONBLOCK);

	if (cevent)
		event_free(cevent);
	if (base)
		event_base_free(base);

#endif
}


int main()
{
	test();
	return 0;
}