#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <iostream>
using namespace std;

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

	std::cout << "test server!\n";
	//����libevent��������
	event_base * base = event_base_new();
	if (base)
	{
		cout << "event_base_new success!" << endl;
	}

	//����˳���
	void Server(event_base*base);
	Server(base);

	//�ͻ��˳���
	void Client(event_base* base);
	Client(base);

	//�¼��ַ�����
	if (base)
		event_base_dispatch(base);
	if (base)
		event_base_free(base);
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
