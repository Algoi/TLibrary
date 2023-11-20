#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#include <string>
#include <event2/thread.h>
#include <thread>
#ifndef _WIN32
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#endif // !_WIN32

using namespace std;

#ifndef _WIN32

void ReadLog(evutil_socket_t fd, short what, void *arg)
{
	char buf[1024] = { 0 };
	int n = read(fd, buf, sizeof buf);
	if (n > 0)
	{
		cout << string(buf, 0, n) << endl;
	}
	else
	{
		cout << "." << flush;
		this_thread::sleep_for(500ms);
	}
}
#endif // !_WIN32

int main()
{
#ifdef _WIN32
	// Windowsƽ̨����Ҫ��ʼ��socket��
	WSADATA wdata;
	WSAStartup(MAKEWORD(2, 2), &wdata);
#else
	// linuxƽ̨�º��Թܵ��źţ��������ݸ��Ѿ��رյ�socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;

	// ��ʼ������
	event_config *config = event_config_new();
	// ���ü����ļ�������
	event_config_require_features(config, EV_FEATURE_FDS);
	// ���������Ķ���
	event_base *base = event_base_new_with_config(config);
	if (base == NULL)
	{
		cerr << "event_base ���������Ķ������..." << endl;
	}
	// �ͷ����ö���
	event_config_free(config);

	// pollģ�ͣ���Ϊ������EV_FEATURE_FDS������epollģ���ǲ�֧�ֵģ����Ը������ȼ�ѡ����pollģ��
	cout << event_base_get_method(base) << endl;

	// ��һ��linux���û���¼��־�ļ�, ��ȡ�ļ��ҷ�������
	int fd = open("/var/log/auth.log", O_RDONLY | O_NONBLOCK);

	// ƫ�Ƶ��ļ�ĩβ��ֻ�����д�����־
	lseek(fd, 0, SEEK_END);

	// �����¼�
	event *revent = event_new(
		base,
		fd,
		EV_READ | EV_PERSIST,
		ReadLog,
		0);

	// ����¼�
	event_add(revent, NULL);

	// �¼��ַ�����
	event_base_dispatch(base);

	// �ͷ�
	if (revent)
		event_free(revent);
	if (base)
		event_base_free(base);

#endif
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}