#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <string.h>
#include <stdlib.h>
#ifndef _WIN32
#include <signal.h>
#else
#endif
using namespace std;

//���󣬳�ʱ �����ӶϿ�����룩
void event_cb(bufferevent *be, short events, void *arg)
{
	//��ʱ�¼����������ݶ�ȡ��ֹͣ
	if (events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
	{
		cout << "timeout..." << endl;
		//����1���˳����ر�socket(����http����������ʱ�Ͽ�����)
		bufferevent_free(be);
		//����2����������ɶ�
		//bufferevent_enable(be, EV_READ);
	}
	else if(events & BEV_EVENT_ERROR)
	{//���������¼�
		bufferevent_free(be);
	}
	else
	{
		cout << "[E]\n" << flush;
	}
	
}

//д�ص�
void write_cb(bufferevent *be, void *arg)
{
	cout << "[W]\n" << flush;
}

//���ص�
void read_cb(bufferevent *be, void *arg)
{
	cout << "[R]\n" << flush;
	char data[1024] = { 0 };

	//��ȡ���뻺������
	int len = bufferevent_read(be, data, sizeof(data) - 1);
	cout << "[" << data << "]" << endl;
	if (len <= 0) return;
	if (strstr(data, "quit") != NULL)
	{
		cout << "quit";
		//�˳����ر�socket BEV_OPT_CLOSE_ON_FREE
		bufferevent_free(be);
	}
	//�������� д�뵽������壬�������ҳ�淢��һ��h1��ǩ����
	string rdata = "HTTP/1.1 200 OK\r\nContent-type:text/html\r\nContent-Length:14\r\n\r\n<h1>HELLO</h1>";
	//bufferevent_write(be, "OK", 3);
	bufferevent_write(be, rdata.data(), rdata.size());
}

//�����ص�
void listen_cb(evconnlistener *ev, evutil_socket_t fd, sockaddr*sin, int slen, void *arg)
{
	cout << "-------------- listen_cb --------------" << endl;

	//�����Ľڵ�
	event_base *base = (event_base *)arg;

	//����bfferevent�������Ķ���BEV_OPT_CLOSE_ON_FREE����buffereventʱ�ر�socket
	bufferevent * bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	//��Ӽ���¼�
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	//1������ˮλ
	//��ȡˮλ����ˮλҪ����յ�����5���ֽڵ����ݲŻᱻ����
	//��ˮλ����Ϊ10��������͵����ݳ�����10����ôÿ�ζ�ȡ�����ݲ��ᳬ��10������ͨ����δ�����ȡ�ص�����
	bufferevent_setwatermark(bev, EV_READ,
		5,		//��ˮλ��0���ǲ����ƣ�Ĭ��Ϊ0
		10		//��ˮλ��0���ǲ����ƣ�Ĭ��Ϊ0
	);

	bufferevent_setwatermark(bev, EV_WRITE,
		5,		//��ˮλ��0���ǲ����ƣ�Ĭ��Ϊ0���������ݵ���5д��ص�����������
		0		//��ˮλ��Ч�������ü���
	);

	//2����ʱʱ�������
	timeval tv = { 3, 0 };
	bufferevent_set_timeouts(bev, &tv, 0);	//���ö�ȡ�¼���ʱʱ��

	//���ûص�����
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, base);
}

int main()
{
#ifdef _WIN32 
	//��ʼ��socket��
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#else
	//���Թܵ��źţ��������ݸ��ѹرյ�socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;
#endif
	//���������Ķ���
	event_base *base = event_base_new();

	//�趨�����Ķ˿ں͵�ַ
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(5001);

	//���Ӽ�����
	evconnlistener *ev = evconnlistener_new_bind(base,
		listen_cb,		//�ص�����
		base,			//�ص������Ĳ���arg
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,				//listen back
		(sockaddr*)&sin,
		sizeof(sin)
	);

	//�����¼���ѭ��
	event_base_dispatch(base);
	evconnlistener_free(ev);
	event_base_free(base);
#ifdef _WIN32
	WSACleanup();
#endif // _WIN32

	return 0;

}

