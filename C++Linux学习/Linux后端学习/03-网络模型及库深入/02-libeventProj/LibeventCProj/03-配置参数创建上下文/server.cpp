#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#include <event2/thread.h>
#ifndef _WIN32
#include<signal.h>
#endif // !_WIN32

using namespace std;

#define PORT 8888

void listen_cb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *)
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

	//-------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------

	//��ʼ������
	event_config * config = event_config_new();

	//1����ʾ֧�ֵ�����ģ��
	const char ** methods = event_get_supported_methods();
	for (int i = 0; methods[i] != NULL; i++)
	{	//�����windowsϵͳ�����һ��win32
		//�����linuxϵͳ�����epoll��poll��select����
		cout << methods[i] << endl;
	}

	//2.1����������
	/**
		event_config_require_features(config, event_method_feature)����
			EV_FEATURE_ET = 0x01, ���ش����ĺ��
			EV_FEATURE_O1 = 0x02, Ҫ����ӡ�ɾ�������¼�������ȷ���ĸ��¼�����Ĳ�����O��1�����Ӷȵĺ��
			EV_FEATURE_FDS = 0x04, Ҫ��֧�������ļ��������������������׽��ֵĺ�ˣ�һ���򿪲���ѡ��epollģ�ͣ�
			EV_FEATURE_EARLY_CLOSE = 0x0��
				������ӹر��¼���������ʹ������������Ӻ�ʱ�رգ������ش������ж�ȡ���й�������ݡ��������к�˶�֧��EV_CLOSED��
				������ʹ��EV_CLOSED������ӹرգ�������Ҫ��ȡ���й�������ݡ�
			//windowsϵͳ�£������ĸ�ȫ��Ĭ�ϲ�֧�֣�Linuxϵͳ�£������ĸ�ֻ�е�������֧��
	*/
	//���õ���������������������֮�󣬻ᵼ�´���������baseʧ�ܡ����Ҽ�����������ᵼ����������Ҳ�޷����á�
	//��linux�����µ����������������óɹ���
	//event_config_require_features(config, EV_FEATURE_FDS);
	//event_config_require_features(config, EV_FEATURE_ET);

	//3.1��Linux��������������ģ��,ʹ��select��windows����ֻ��win32
	/*event_config_avoid_method(config, "epoll");
	event_config_avoid_method(config, "poll");*/

	//4.1��windows��֧��IOCP���ã��̳߳أ���һ��ʹ��IOCP������ʱ�ͻῪ���̳߳�
#ifdef _WIN32
	event_config_set_flag(config, EVENT_BASE_FLAG_STARTUP_IOCP);
	//��ʼ��iocp���߳�
	evthread_use_windows_threads();
	//����cpu����
	SYSTEM_INFO sinfo;
	GetSystemInfo(&sinfo);	//��ȡ�������Ϣ
	event_config_set_num_cpus_hint(config, sinfo.dwNumberOfProcessors);
#endif // _WIN32

	//--------------------------------------------------------------------------------------
	//���������ģ���Ҫͨ�����ô���
	event_base * base = event_base_new_with_config(config);
	if (base == NULL) {
		//���ͨ���Զ������ô���ʧ�ܣ���ô��ͨ��Ĭ�����ô���
		cout << "���ô���������ʧ��" << endl;
		base = event_base_new();
		if (base == NULL) {
			return 0;
		}
	}
	event_config_free(config);

	//3.2����ȡ��ǰ�����ĵ�����ģ��
	//windowsƽ̨�¾���win32��linuxƽ̨����epoll��Ĭ�ϣ�
	const char * curmethod = event_base_get_method(base);
	cout << curmethod << endl;

	//2.2��ȷ�������Ƿ���Ч
	int confrim = event_base_get_features(base);
	if (confrim & EV_FEATURE_ET)
	{
		cout << "EV_FEATURE_ET ���ش���" << endl;
	}
	else {
		cout << "δ���ñ��ش���" << endl;
	}
	if (confrim & EV_FEATURE_O1)
	{
		cout << "EV_FEATURE_O1" << endl;
	}
	else {
		cout << "δ����EV_FEATURE_O1" << endl;
	}
	if (confrim & EV_FEATURE_FDS)
	{
		cout << "EV_FEATURE_FDS" << endl;
	}
	else {
		cout << "δ����EV_FEATURE_FDS" << endl;
	}
	if (confrim & EV_FEATURE_EARLY_CLOSE)
	{
		cout << "EV_FEATURE_EARLY_CLOSE" << endl;
	}
	else {
		cout << "δ����EV_FEATURE_EARLY_CLOSE" << endl;
	}

	//-------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------

	sockaddr_in saddr;
	memset(&saddr, 0x0, sizeof saddr);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);

	//����������
	evconnlistener * listener = evconnlistener_new_bind(
		base,				
		listen_cb, base,	
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,						
		(sockaddr *)&saddr,		
		sizeof(saddr)			
	);

	//�¼��ַ�����
	event_base_dispatch(base);

	if (listener)
		evconnlistener_free(listener);
	if (base)
		event_base_free(base);

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}