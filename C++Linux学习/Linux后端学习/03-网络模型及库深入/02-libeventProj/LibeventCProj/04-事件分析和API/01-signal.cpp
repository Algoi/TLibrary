#include <iostream>
#include <event2/event.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif // !_WIN32

using namespace std;

#define PORT 8888

void ctrl_c_cb(int sock, short which, void *arg)
{
	cout << "sock: " << sock << endl;
	cout << "which: " << which << endl;
	if (which & EV_TIMEOUT)
	{
		cout << "EV_TIMEOUT" << endl;
	}
	if (which & EV_SIGNAL)
	{
		cout << "EV_SIGNAL" << endl;	//ֻ����������
	}
	if (which & EV_PERSIST)
	{
		cout << "EV_PERSIST" << endl;
	}
	if (which & EV_ET)
	{
		cout << "EV_ET" << endl;
	}
}

void Kill_cb(int sock, short which, void *arg)
{
	cout << "kill " << endl;
	//������ڷǴ���״̬
	event * ev = (struct event *)arg;
	if (!evsignal_pending(ev, NULL))
	{
		event_del(ev);
		event_add(ev, NULL);
	}
}

void test()
{
#ifdef _WIN32
#else
	// linuxƽ̨�º��Թܵ��źţ��������ݸ��Ѿ��رյ�socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;

	event_base *base = event_base_new();

	/*
		1��ʹ���ź��¼���Ӧ��API��
			#define evsignal_add(ev, tv)			event_add((ev), (tv))
			#define evsignal_new(b, x, cb, arg)		event_new((b), (x), EV_SIGNAL|EV_PERSIST, (cb), (arg))
			#define evsignal_del(ev)				event_del(ev)
			#define evsignal_pending(ev, tv)		event_pending((ev), EV_SIGNAL, (tv))
	*/
	// ���ctrl + c
	event *cevent = evsignal_new(base, SIGINT, ctrl_c_cb, base);
	if (cevent == NULL)
	{
		cout << "evsignal failed..." << endl;
		return -1;
	}
	// ����¼�
	if (evsignal_add(cevent, 0) != 0)
	{
		cout << "event add failed..." << endl;
	}

	/*
		2��ʹ��event_new�����ź��¼������kill�ź�

		����event_self_cbarg()�����ݵ�ǰ��ksigevent����
	*/
	event * ksigevent = event_new(base, SIGTERM, EV_SIGNAL, Kill_cb, event_self_cbarg());

	event_add(ksigevent, NULL);

	// ���¼�ѭ��
	//event_base_dispatch(base);
	//EVLOOP_ONCE �ȴ�һ���¼����У�ֱ��û�л���¼����˳�������������ź��¼�����������һ���źţ�ִ����ص�֮��ͻ����ѭ���˳�����
	//EVLOOP_NONBLOCK  �л�¼�����û�л�¼������̷���0�������˳������������ﶼ���ź��¼�����������ʱû���κ��¼���Ҫ������ô��ֱ���˳��˳��򡣿������ѭ��ʹ��
	//EVLOOP_NO_EXIT_ON_EMPTY  û������¼���event_add��Ҳ�����ء������������ڶ��߳�������¼���
	//event_base_dispatch(base) = event_base_loop(base, 0); ֻҪ���¼���ӵ���event_base�������У���ô��һֱѭ��������¼�û�б���ӵ�base�У���ôֱ���˳�ѭ����
	event_base_loop(BASETYPES, EVLOOP_NONBLOCK);

	if (cevent)
		event_free(cevent);
	if (base)
		event_base_free(base);

#endif
}


int main()
{
	test();
	return 0;
}