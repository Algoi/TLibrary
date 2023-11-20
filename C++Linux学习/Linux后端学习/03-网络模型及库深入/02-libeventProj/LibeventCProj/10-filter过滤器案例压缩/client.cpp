/*/*******************************************************************************
**                                                                            **
**                     Jiedi(China nanjing)Ltd.                               **
**	               ������������ �Ĳܿ����˴��������Ϊѧϰ�ο�                **
*******************************************************************************/

/*****************************FILE INFOMATION***********************************
**
** Project       : Libevent C�����߲���������
** Contact       : xiacaojun@qq.com
**  ����   : http://blog.csdn.net/jiedichina
**	��Ƶ�γ� : �����ƿ���	http://study.163.com/u/xiacaojun
			   ��Ѷ����		https://jiedi.ke.qq.com/
			   csdnѧԺ		http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961
**             51ctoѧԺ	http://edu.51cto.com/lecturer/index/user_id-12016059.html
** 			   ���Ŀ���		http://www.laoxiaketang.com
**
**  Libevent C�����߲��������� �γ�Ⱥ ��1003847950 ����Ⱥ���ش���ͽ���
**   ΢�Ź��ں�  : jiedi2007
**		ͷ����	 : �Ĳܿ�
**
*****************************************************************************
//������������������ Libevent C�����߲��������� �γ�  QQȺ��1003847950 ���ش���ͽ���*/
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <iostream>
using namespace std;
#define FILEPATH "001.bmp"

struct ClientStatus
{
	FILE *fp = 0;
	bool end = false;
};
bufferevent_filter_result filter_out(evbuffer *s, evbuffer *d,
	ev_ssize_t limit, bufferevent_flush_mode mode, void *arg)
{

	//cout << "filter_out" << endl;
	char data[1024] = { 0 };
	int len = evbuffer_remove(s, data, sizeof(data));
	evbuffer_add(d, data, len);
	return BEV_OK;
}

static void client_read_cb(bufferevent *bev, void *arg)
{
	//002 ���շ���˷��͵�OK�ظ�
	char data[1024] = { 0 };
	int len = bufferevent_read(bev, data, sizeof(data) - 1);
	if (strcmp(data, "OK") == 0)
	{
		cout << data << endl;

		//��ʼ�����ļ�,����д��ص�
		bufferevent_trigger(bev, EV_WRITE, 0);
	}
	else
	{
		bufferevent_free(bev);
	}
	cout << "client_read_cb " << len << endl;
}
static void client_write_cb(bufferevent *bev, void *arg)
{
	ClientStatus *s = (ClientStatus *)arg;
	FILE *fp = s->fp;
	//�ж�ʲôʱ��������Դ
	if (s->end)
	{
		//�жϻ����Ƿ������ݣ������ˢ��
		//��ȡ�������󶨵�buffer
		bufferevent *be = bufferevent_get_underlying(bev);
		//��ȡ������弰���С
		evbuffer *evb = bufferevent_get_output(be);
		int len = evbuffer_get_length(evb);
		//cout << "evbuffer_get_length = " << len << endl;
		if (len <= 0)
		{
			//�������� ������������ݣ����ᷢ��
			bufferevent_free(bev);
			delete s;
			return;
		}
		//ˢ�»���
		bufferevent_flush(bev, EV_WRITE, BEV_FINISHED);
		return;
	}

	if (!fp)return;
	//cout << "client_write_cb" << endl;
	//��ȡ�ļ�
	char data[1024] = { 0 };
	int len = fread(data, 1, sizeof(data), fp);
	if (len <= 0)
	{
		fclose(fp);
		s->end = true;
		//ˢ�»���
		bufferevent_flush(bev, EV_WRITE, BEV_FINISHED);
		return;
	}
	//�����ļ�
	bufferevent_write(bev, data, len);
}
static void client_event_cb(bufferevent*be, short events, void *arg)
{
	cout << "client_event_cb " << events << endl;
	if (events & BEV_EVENT_CONNECTED)
	{
		cout << "BEV_EVENT_CONNECTED" << endl;
		//001 �����ļ���
		bufferevent_write(be, FILEPATH, strlen(FILEPATH));

		//�����������
		bufferevent * bev_filter = bufferevent_filter_new(be, 0, filter_out,
			BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS, 0, 0);
		FILE *fp = fopen(FILEPATH, "rb");
		if (!fp)
		{
			cout << "open file " << FILEPATH << " failed!" << endl;
			//return;
		}
		ClientStatus *s = new ClientStatus();
		s->fp = fp;

		//���ö�ȡ��д����¼��Ļص�
		bufferevent_setcb(bev_filter, client_read_cb, client_write_cb, client_event_cb, s);
		bufferevent_enable(bev_filter, EV_READ | EV_WRITE);
	}
}

void Client(event_base* base)
{
	cout << "begin Client" << endl;
	//���ӷ����
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(5001);
	evutil_inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr.s_addr);
	bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

	//ֻ���¼��ص�������ȷ�����ӳɹ�
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	bufferevent_setcb(bev, 0, 0, client_event_cb, 0);

	bufferevent_socket_connect(bev, (sockaddr*)&sin, sizeof(sin));
	//���ջظ�ȷ��OK
}