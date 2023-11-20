/*
1、主事件循环函数
	int event_base_loop(struct event_base *base,int flags);
	#define EVLOOP_ONCE 0x01				等待一个事件运行，直到没有活动的事件就退出
	#define EVLOOP_NONBLOCK 0x02			有活动事件处理，没有活动事件，立刻返回
	#define EVLOOP_NO_EXIT_ON_EMPTY 0x04	没有添加事件也不返回	event_base_dispatch(base) = event_base_loop(base, 0); 只要有事件添加到了event_base上下文中，那么就一直循环。如果事件没有被添加到base中，那么直接退出循环。*//*	int event_base_loopexit(struct event_base *	base, const struct timeval *tv);
	运行完所有激活事件(事件被触发)的回调之才退出事件循环没有运行时，下一轮回调完成后立即停止
	tv表示至少至少要运行指定的时间才退出

	int event_base_loopbreak(struct event_base * base);
	执行完当前正在处理的事件后立即退出，则立即退出*/