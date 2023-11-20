
/*
关于上下文event_base对象

1、通过event_base *event_base_new(void)函数创建默认配置属性的上下文对象

2、void event_base_free(struct event_base *)
		释放event_base内部分配的空间及其本身对象的空间，不释放事件和socket和在回调函数中申请的空间

3、event_base *event_base_new_with_config(const struct event_config *);
	通过event_config对象来创建上下文对象

4、event_reinit int event_reinit(struct event_base *base); 在fork的子进程中重新初始化上下文
*/

/*
关于配置属性对象 struct event_config

1、event_config *event_config_new(void):				创建event_config对象
2、void event_config_free(struct event_config *cfg)：	释放event_config对象

//设置config属性
3、event_config_require_features(struct event_config *cfg, event_method_feature):设置config特征
		EV_FEATURE_ET = 0x01, 边沿触发的后端
		EV_FEATURE_O1 = 0x02, 要求添加、删除单个事件，或者确定哪个事件激活的操作是O（1）复杂度的后端
		EV_FEATURE_FDS = 0x04, 要求支持任意文件描述符，而不仅仅是套接字的后端（一旦打开不能选择epoll模型）
		EV_FEATURE_EARLY_CLOSE = 0x0：
			检测连接关闭事件。您可以使用它来检测连接何时关闭，而不必从连接中读取所有挂起的数据。并非所有后端都支持EV_CLOSED。
			允许您使用EV_CLOSED检测连接关闭，而不需要读取所有挂起的数据。
		//windows系统下，以上四个全部默认不支持；Linux系统下，以上四个只有第三个不支持

4、event_config_avoid_method(struct event_config *cfg, const char *method);
		取消网络模型，windows环境只有win32，比如在linux环境下设置select网络模型：
		event_config_avoid_method(config, "epoll");
		event_config_avoid_method(config, "poll");

5、event_config_set_flag(struct event_config *cfg, int flag)
		EVENT_BASE_FLAG_NOLOCK : 不为上下文对象event_base分配锁，在单线程程序或者在外部进行加锁的情况下，可以设置这个标志。
		EVENT_BASE_FLAG_STARTUP_IOCP : 用于在Windows平台上启用IOCP事件处理模式，以提高网络IO性能
			{
					event_config_set_flag(config, EVENT_BASE_FLAG_STARTUP_IOCP);
					//初始化iocp的线程
					evthread_use_windows_threads();
					//设置cpu数量
					SYSTEM_INFO sinfo;
					GetSystemInfo(&sinfo);	//获取计算机信息
					event_config_set_num_cpus_hint(config, sinfo.dwNumberOfProcessors);
			}
		EVENT_BASE_FLAG_IGNORE_ENV : 用于忽略环境变量，即在libevent库启动过程中，不使用环境变量来配置库的行为。
		EVENT_BASE_FLAG_NO_CACHE_TIME : 不是在事件循环每次准备执行超时回调时检测当前时间，而是在每次超时回调后进行检测。
										注意：这会消耗更多的CPU时间
		EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST : epoll下有效，防止同一个fd多次激发事件，fd如果做复制会有bug
		EVENT_BASE_FLAG_PRECISE_TIMER : 默认使用系统最快的记时机制，如果系统有较慢且更精确的则采用。

*/