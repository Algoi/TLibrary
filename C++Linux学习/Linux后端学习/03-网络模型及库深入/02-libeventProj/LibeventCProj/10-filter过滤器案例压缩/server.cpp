#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <iostream>
#include <string>
using namespace std;

#define SPORT 5001

struct Status
{
	bool start = false;
	FILE *fp = 0;
	//string filename;
};

//���������
bufferevent_filter_result filter_in(evbuffer *s, evbuffer *d,
	ev_ssize_t limit, bufferevent_flush_mode mode, void *arg)
{
	//1 ���տͻ��˷��͵��ļ���
	char data[1024] = { 0 };
	int len = evbuffer_remove(s, data, sizeof(data) - 1);

	//2����������ص����������ݣ����ļ�������ȥ
	evbuffer_add(d, data, len);
	return BEV_OK;
}

//���ص�����
static void read_cb(bufferevent *bev, void *arg)
{
	Status *status = (Status *)arg;
	if (!status->start)
	{
		//001�����ļ���
		char data[1024] = { 0 };
		bufferevent_read(bev, data, sizeof(data) - 1);
		//status->filename = data;
		string out = "out\\";
		out += data;
		//��д���ļ�
		status->fp = fopen(out.c_str(), "wb");
		if (!status->fp)
		{
			cout << "server open " << out << " failed!" << endl;
			return;
		}

		//002 �ظ�OK
		bufferevent_write(bev, "OK", 2);
		status->start = true;
		return;
	}

	do
	{
		//д���ļ�
		char data[1024] = { 0 };
		int len = bufferevent_read(bev, data, sizeof(data));
		if (len >= 0)
		{
			fwrite(data, 1, len, status->fp);
			fflush(status->fp);
		}	//ֻҪ���������е����ݳ��ȴ���0��һֱ��
	} while (evbuffer_get_length(bufferevent_get_input(bev)) > 0);
}

//�¼��ص�����
static void event_cb(bufferevent *bev, short events, void *arg)
{
	cout << "server event_cb " << events << endl;
	Status *status = (Status *)arg;
	if (events & BEV_EVENT_EOF)
	{
		cout << "server event BEV_EVENT_EOF" << endl;
		if (status->fp)
		{
			fclose(status->fp);
			status->fp = 0;
		}
		bufferevent_free(bev);
	}
}

//�����ص�����
static void listen_cb(struct evconnlistener * e, evutil_socket_t s, struct sockaddr *a, int socklen, void *arg)
{
	cout << "listen_cb" << endl;
	event_base *base = (event_base *)arg;
	//1 ����һ��bufferevent ����ͨ��
	bufferevent *bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);
	Status *status = new Status();

	//2 ��ӹ��� ����������ص�
	bufferevent *bev_filter = bufferevent_filter_new(bev,
		filter_in,//������˺���
		0,//�������
		BEV_OPT_CLOSE_ON_FREE,//�ر�filterͬʱ����bufferevent
		0, //����ص�
		status	//���ݲ���
	);

	//3 ���ûص� ��ȡ �¼����������ӶϿ��� 
	bufferevent_setcb(bev_filter, read_cb, 0, event_cb, status);
	bufferevent_enable(bev_filter, EV_READ | EV_WRITE);
}

//�����������
void Server(event_base*base)
{
	cout << "begin Server" << endl;
	//�����˿�
//socket ��bind��listen ���¼�
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);

	evconnlistener *ev = evconnlistener_new_bind(base,	// libevent��������
		listen_cb,					//���յ����ӵĻص�����
		base,						//�ص�������ȡ�Ĳ��� arg
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,//��ַ���ã�evconnlistener�ر�ͬʱ�ر�socket
		10,							//���Ӷ��д�С����Ӧlisten����
		(sockaddr*)&sin,							//�󶨵ĵ�ַ�Ͷ˿�
		sizeof(sin)
	);

}