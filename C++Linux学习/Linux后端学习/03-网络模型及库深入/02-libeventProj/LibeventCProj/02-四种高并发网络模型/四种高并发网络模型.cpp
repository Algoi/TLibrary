
/*
1、select 
	跨平台
	每次都要从用户空间拷贝到内核空间
	遍历整个fd_set
	最大可监听的fd数量不能找过FD_SETSIZE
2、poll
	基本类似与select，无FD_SETSIZE限制
3、epoll
	缺点：不支持windows
	特点：
		内核态：红黑树
		共享内存交互mmap
		不需要全部复制，返回双向链表
		LT水平触发。只要有可读数据，就一直通知
		ET边沿触发。只通知一次，不管数据是否读完
		需要自己做线程池，如果数据量很大
4、iocp
	linux不支持
	支持线程池

libevent中四种模型的优先级：epoll > poll > select > iocp
*/
/*
libevent接口分析
1、环境配置和初始化
	event_base_new
2、evutil socket函数封装
	evutil_make_sock_nonblocking
	evutil_make_listen_socket_reuseable
	evutil_closesocket
3、事件io处理
	event_new  需要传递socket或者其他文件描述符
4、缓冲IO
	bufferevent
5、事件循环
	event_base_dispatch

*/