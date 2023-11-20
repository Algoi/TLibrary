#include <iostream>
#include <event2/event.h>
#ifndef _WIN32
#include <signal.h>
#else
#endif
#include <thread>
#include <errno.h>
#include <string.h>
using namespace std;

#define SPORT 5001 

//�����Ͽ�����Ҳ����룬��ʱ�����
void client_cb(evutil_socket_t s, short w, void *arg)
{
	//ˮƽ����LT ֻ��������û�д�����һֱ����
	//��Ե����ET ������ʱֻ����һ��
	//cout<<"."<<flush;return;

	event *ev = (event *)arg;
	//�жϳ�ʱ
	if (w & EV_TIMEOUT)
	{
		cout << "timeout" << endl;
		event_free(ev);
		evutil_closesocket(s);
		return;
	}

	//char buf[1024] = {0};
	char buf[1024] = { 0 };
	int len = recv(s, buf, sizeof(buf) - 1, 0);
	if (len > 0)
	{
		cout << buf << endl;
		send(s, "ok", 2, 0);
	}
	else
	{
		//���ͻ��������Ͽ����ӻ�������
		//��Ҫ����event
		cout << "event_free" << flush;
		event_free(ev);
		evutil_closesocket(s);

	}

}

void listen_cb(evutil_socket_t s, short w, void *arg)
{
	cout << "listen_cb" << endl;
	sockaddr_in sin;
	socklen_t size = sizeof(sin);
	//��ȡ������Ϣ
	evutil_socket_t client = accept(s, (sockaddr*)&sin, &size);
	char ip[16] = { 0 };
	evutil_inet_ntop(AF_INET, &sin.sin_addr, ip, sizeof(ip) - 1);
	cout << "client ip is " << ip << endl;

	//�ͻ������ݶ�ȡ�¼�
	event_base *base = (event_base *)arg;
	event *ev = event_new(base, client, EV_READ | EV_PERSIST, client_cb, event_self_cbarg());
	//��Ե����
	//event *ev = event_new(base,client,EV_READ|EV_PERSIST|EV_ET,client_cb,event_self_cbarg());
	timeval t = { 10,0 };
	event_add(ev, &t);
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

	//event ������
	cout << "test event server" << endl;

	//����socket
	evutil_socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock <= 0)
	{
		cerr << "socket error��" << strerror(errno) << endl;
		return -1;
	}
	int val = 1;

	//���õ�ַ���úͷ�����
	evutil_make_socket_nonblocking(sock);
	evutil_make_listen_socket_reuseable(sock);
	//�󶨶˿ں͵�ַ
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);
	int re = ::bind(sock, (sockaddr*)&sin, sizeof(sin));
	if (re != 0)
	{
		cerr << "bind error:" << strerror(errno) << endl;
		return -1;
	}
	//��ʼ����
	listen(sock, 10);

	//��ʼ���������¼� Ĭ��ˮƽ����
	event *ev = event_new(base, sock, EV_READ | EV_PERSIST, listen_cb, base);
	event_add(ev, 0);


	//�����¼���ѭ��
	event_base_dispatch(base);
	evutil_closesocket(sock);
	event_base_free(base);

	return 0;
}