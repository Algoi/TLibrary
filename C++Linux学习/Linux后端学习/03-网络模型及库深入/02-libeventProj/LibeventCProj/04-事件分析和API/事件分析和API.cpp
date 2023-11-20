/*

1、事件状态分析:
	1.1 已初始化（initialized） 调用event_new之后
	1.2 待决（pending） 调用event_add之后 只有此状态才会调用事件回调
	1.3 激活（active） 事件发生，包括超时事件
	1.4 持久的（persistent） 
		如果没设置持久每次事件函数调用后，状态就变为非待决，也就收不到事件了，需要再次调用event_add。

2、事件API分析
	2.1 struct event *event_new(struct event_base *base, evutil_socket_t fd, short what, event_callback_fn cb, void *arg);
		功能：初始化event事件对象，通过返回值返回。实现事件的初始化状态
		参数：
			base：上下文节点
			fd：监听文件描述符
			what：事件标志
				EV_TIMEOUT	超时事件，默认是忽略的，只有在添加事件时设置超时事件才有效
				EV_READ		读事件
				EV_WRITE	写事件
				EV_SIGNAL	信号事件
				EV_PERSIST	持久。若是不加，那么事件就是一次触发之后就不再触发，需要再次添加该事件才能触发
				EV_ET		边缘触发，需要底层支持，2.0以上支持，影响EV_READ和EV_WRITE
			cb：回调函数，事件触发时执行的函数。原型：void(*event_callback_fn)(evutil_socket_t fd, short what, void *arg);
				fd：发生事件的文件描述符
				what：事件标志
				arg：传递过来的参数，同event_new函数的最后一个参数
			arg：给回调函数传递的参数
	2.2 int event_add(struct event *ev, const struct timeval *tv)
		功能：添加事件，此后事件将会转为待决状态。成功返回0，失败返回-1
		参数：
			ev：需要添加的事件对象
			tv：超时时间，可以直接传NULL。做定时器时需要设置具体的
	2.3 int event_del(struct event *ev);
		功能：删除参数指定的事件对象，只是将事件状态变成了非待决状态，可以通过event_add再次添加变为待决状态
	2.4 void event_free(struct event *event)
		功能：已待决和激活状态调用，清理空间前会使得状态变为未待决和非激活
	2.5 int event_pending(const struct event *ev, short events, struct timeval *tv);
		功能：判断指定的事件是否处于待决状态
*/

/*
关于信号事件API宏
	#define evsignal_add(ev, tv)			event_add((ev), (tv))
	#define evsignal_new(b, x, cb, arg)		event_new((b), (x), EV_SIGNAL|EV_PERSIST, (cb), (arg))
	#define evsignal_del(ev)				event_del(ev)
	#define evsignal_pending(ev, tv)		event_pending((ev), EV_SIGNAL, (tv))
*/

/*
关于定时器事件API宏
	#define evtimer_new(b, cb, arg)		event_new((b), -1, 0, (cb), (arg))
			0：表示不传任何的事件标志
	#define evtimer_add(ev, tv)			event_add((ev), (tv))
	#define evtimer_del(ev)				event_del(ev)
	#define evtimer_pending(ev, tv)		event_pending((ev), EV_TIMEOUT, (tv))

	超时性能优化
	const struct timeval *event_base_init_common_timeout(
						struct event_base *base,
						const struct timeval *duration);
*/