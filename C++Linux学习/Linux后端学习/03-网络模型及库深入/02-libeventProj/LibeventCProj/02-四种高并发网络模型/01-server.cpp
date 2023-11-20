#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#ifndef _WIN32
#include<signal.h>
#endif // !_WIN32

using namespace std;

#define PORT 8888

void listen_cb(struct evconnlistener * listener, evutil_socket_t fd, struct sockaddr * saddr, int socklen, void *arg)
{
	cout << "listen_cb" << endl;
}

int main()
{
#ifdef _WIN32
	//Windowsƽ̨����Ҫ��ʼ��socket��
	WSADATA wdata;
	WSAStartup(MAKEWORD(2, 2), &wdata);
#else
	//linuxƽ̨�º��Թܵ��źţ��������ݸ��Ѿ��رյ�socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;
#endif

	cout << "test server..." << endl;
	event_base * base = event_base_new();
	if (base)
	{
		cout << "event_base success..." << endl;
	}
	
	sockaddr_in saddr;
	memset(&saddr, 0x0, sizeof saddr);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);
	//��������������װ��socket��bind��listen�Լ�accept����
	evconnlistener * listener = evconnlistener_new_bind(
		base,				//libevent�������ģ����ڵ�
		listen_cb, base,	//���յ�����ʱ�Ļص�����(�������Ӿͻᴥ��)���������
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, //���ö˿ڸ����Լ�listener�ر�ʱ�رն�Ӧ�ļ���socket�ļ�������
		10,						//���Ӷ��д�С��listen�����ĵڶ�������
		(sockaddr *)&saddr,		//�󶨵ĵ�ַ�Ͷ˿�
		sizeof(saddr)			//saddr���ֽڴ�С
		);
	//�¼��ַ�����
	event_base_dispatch(base);

	if (listener)
		evconnlistener_free(listener);
	if(base)
		event_base_free(base);

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}