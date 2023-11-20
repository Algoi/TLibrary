#include <iostream>
#include <event2/event.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif // !_WIN32
using namespace std;

static timeval tv1 = { 1, 0 };

void timer1(int sock, short which, void * arg)
{
	cout << "timer1 triggled..." << endl;
	struct event * ev = (struct event *)arg;
	if (!evtimer_pending(ev, &tv1))
	{//�ٴ���ӵ��������еȴ���ʱ����
		evtimer_del(ev);
		evtimer_add(ev, &tv1);
	}
}

void timer2(int sock, short which, void * arg)
{
	cout << "timer2 triggled..." << endl;
}

void timer3(int sock, short which, void * arg)
{
	cout << "timer3 triggled..." << endl;
}

void test1()
{
#ifdef _WIN32
	//��ʼ��socket��
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#else
	// linuxƽ̨�º��Թܵ��źţ��������ݸ��Ѿ��رյ�socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;
#endif
	//�����Ķ���
	event_base *base = event_base_new();

	/*
		1��ʹ�ù��ڶ�ʱ����صĺ�
		#define evtimer_new(b, cb, arg)		event_new((b), -1, 0, (cb), (arg))
		#define evtimer_add(ev, tv)			event_add((ev), (tv))
		#define evtimer_del(ev)				event_del(ev)
		#define evtimer_pending(ev, tv)		event_pending((ev), EV_TIMEOUT, (tv))
	*/
	//��ʱ����Ĭ���Ƿǳ־õ��¼�����ʱֻ�ᴥ��һ��
	cout << " test timer " << endl;
	event * evl = evtimer_new(base, timer1, event_self_cbarg());
	if (!evl) {
		cout << "evtimer_new failed..." << endl;
	}
	evtimer_add(evl, &tv1);

	/*
		2��ֱ��ʹ��event_xxx��غ���
	*/
	static struct timeval tv2 = { 1, 200000 };
	event * ev2 = event_new(base, -1, EV_PERSIST, timer2, 0);
	event_add(ev2, &tv2);

	/*
		3����ʱ�����Ż�
		��ʱ�Ż����ܣ�Ĭ��event�ö���Ѵ洢����ȫ�������� ����ɾ��0(logN)
		�Ż���˫����� ����ɾ��0(1)
	*/
	event * ev3 = event_new(base, -1, EV_PERSIST, timer3, 0);
	static timeval tv_in = { 3, 0 };
	const timeval * t3;
	t3 = event_base_init_common_timeout(base, &tv_in);
	event_add(ev3, t3);		//��������0(1)

	// �¼��ַ�
	event_base_dispatch(base);

	if (base)
		event_base_free(base);
}

//int main()
//{
//	test1();
//
//	return 0;
//}