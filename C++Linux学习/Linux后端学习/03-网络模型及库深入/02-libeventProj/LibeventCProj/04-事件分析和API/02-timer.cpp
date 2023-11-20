#include <iostream>
#include <event2/event.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif // !_WIN32
using namespace std;

static timeval tv1 = { 1, 0 };

void timer1(int sock, short which, void * arg)
{
	cout << "timer1 triggled..." << endl;
	struct event * ev = (struct event *)arg;
	if (!evtimer_pending(ev, &tv1))
	{//再次添加到上下文中等待超时触发
		evtimer_del(ev);
		evtimer_add(ev, &tv1);
	}
}

void timer2(int sock, short which, void * arg)
{
	cout << "timer2 triggled..." << endl;
}

void timer3(int sock, short which, void * arg)
{
	cout << "timer3 triggled..." << endl;
}

void test1()
{
#ifdef _WIN32
	//初始化socket库
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#else
	// linux平台下忽略管道信号，发送数据给已经关闭的socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;
#endif
	//上下文对象
	event_base *base = event_base_new();

	/*
		1、使用关于定时器相关的宏
		#define evtimer_new(b, cb, arg)		event_new((b), -1, 0, (cb), (arg))
		#define evtimer_add(ev, tv)			event_add((ev), (tv))
		#define evtimer_del(ev)				event_del(ev)
		#define evtimer_pending(ev, tv)		event_pending((ev), EV_TIMEOUT, (tv))
	*/
	//定时器。默认是非持久的事件，超时只会触发一次
	cout << " test timer " << endl;
	event * evl = evtimer_new(base, timer1, event_self_cbarg());
	if (!evl) {
		cout << "evtimer_new failed..." << endl;
	}
	evtimer_add(evl, &tv1);

	/*
		2、直接使用event_xxx相关函数
	*/
	static struct timeval tv2 = { 1, 200000 };
	event * ev2 = event_new(base, -1, EV_PERSIST, timer2, 0);
	event_add(ev2, &tv2);

	/*
		3、超时性能优化
		超时优化性能，默认event用二叉堆存储（完全二叉树） 插入删除0(logN)
		优化到双向队列 插入删除0(1)
	*/
	event * ev3 = event_new(base, -1, EV_PERSIST, timer3, 0);
	static timeval tv_in = { 3, 0 };
	const timeval * t3;
	t3 = event_base_init_common_timeout(base, &tv_in);
	event_add(ev3, t3);		//插入性能0(1)

	// 事件分发
	event_base_dispatch(base);

	if (base)
		event_base_free(base);
}

//int main()
//{
//	test1();
//
//	return 0;
//}