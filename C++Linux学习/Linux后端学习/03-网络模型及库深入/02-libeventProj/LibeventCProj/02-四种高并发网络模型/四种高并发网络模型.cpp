
/*
1��select 
	��ƽ̨
	ÿ�ζ�Ҫ���û��ռ俽�����ں˿ռ�
	��������fd_set
	���ɼ�����fd���������ҹ�FD_SETSIZE
2��poll
	����������select����FD_SETSIZE����
3��epoll
	ȱ�㣺��֧��windows
	�ص㣺
		�ں�̬�������
		�����ڴ潻��mmap
		����Ҫȫ�����ƣ�����˫������
		LTˮƽ������ֻҪ�пɶ����ݣ���һֱ֪ͨ
		ET���ش�����ֻ֪ͨһ�Σ����������Ƿ����
		��Ҫ�Լ����̳߳أ�����������ܴ�
4��iocp
	linux��֧��
	֧���̳߳�

libevent������ģ�͵����ȼ���epoll > poll > select > iocp
*/
/*
libevent�ӿڷ���
1���������úͳ�ʼ��
	event_base_new
2��evutil socket������װ
	evutil_make_sock_nonblocking
	evutil_make_listen_socket_reuseable
	evutil_closesocket
3���¼�io����
	event_new  ��Ҫ����socket���������ļ�������
4������IO
	bufferevent
5���¼�ѭ��
	event_base_dispatch

*/