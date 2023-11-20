#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#else
#endif
#include <string>
using namespace std;

//����ȡ�������ݱ�������
static string recvData = "";

//�¼��ص�
void event_cb(bufferevent *be, short events, void *arg)
{
	cout << "client Event CB" << endl;

	//��ȡ��ʱ�¼��������ȡ����ֹͣ
	if (events && BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
	{
		//��ȡ�����е�����
		char data[1024] = { 0 };
		//��ȡ���绺�����е����ݣ���Ϊ������ˮλ�����ܻ������ݲ����ڻ�����
		int len = bufferevent_read(be, data, sizeof(data) - 1);
		if (len > 0)
		{
			recvData += data;
		}

		cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << endl;
		//�ͷ��¼�
		bufferevent_free(be);
		//������յ������ݣ�һ�������
		cout << recvData << endl;
	}
	else if (events & BEV_EVENT_ERROR)
	{
		bufferevent_free(be);
	}
	else
	{
		cout << "others" << endl;
	}
}

//д�ص�
void write_cb(bufferevent *be, void *arg)
{
	cout << "Write CB" << endl;
}

//���ص�
void read_cb(bufferevent *be, void *arg)
{
	cout << "Read CB" << endl;

	char data[1024] = { 0 };
	//��ȡ�������뻺�����е�����
	int len = bufferevent_read(be, data, sizeof(data) - 1);

	if (len <= 0) return;

	//��������
	recvData += data;

	//�������ݣ�д�뵽�������������
	bufferevent_write(be, "OK", 3);
}

//���ӻص�����
void listen_cb(struct evconnlistener *, evutil_socket_t fd, struct sockaddr * addr, int socklen, void * arg)
{
	cout << "listen CB" << endl;

	//�����Ķ���
	event_base * base = (event_base *)arg;

	//����bufferevent�����Ķ��� BEV_OPT_CLOSE_ON_FREE����buffereventʱ�ر�socket
	bufferevent * bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	//��Ӽ���¼�
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	//����ˮλ
	//��ȡˮλ
	bufferevent_setwatermark(bev, EV_READ,
		5,	//��ˮλ 0���������� Ĭ����0
		10	//��ˮλ 0���������� Ĭ����0
	);

	bufferevent_setwatermark(bev, EV_WRITE,
		5,	//��ˮλ 0���������� Ĭ����0 �������ݵ���5 д��ص�������
		0	//��ˮλ��Ч
	);

	//���ó�ʱʱ��
	timeval tv = { 0, 500000 };
	//���ö�ȡ�ĳ�ʱʱ��
	bufferevent_set_timeouts(bev, &tv, 0);

	//���ûص�����
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, base);
}

int main(int argc, char *argv[])
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
	//�����Ķ���
	event_base *base = event_base_new();

	//�������������
	//�趨�����Ķ˿ں͵�ַ
	sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(5001);

	//���Ӽ�����
	evconnlistener * listener = evconnlistener_new_bind(
		base,
		listen_cb,
		base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,
		(sockaddr *)&saddr,
		sizeof saddr
		);

	//�����¼���ѭ��
	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_base_free(base);

	return 0;
}