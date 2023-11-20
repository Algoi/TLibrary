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

//���󣬳�ʱ�����ӶϿ�����룩
void client_event_cb(bufferevent *be, short events, void *arg)
{
	cout << "client Event CB" << endl;

	//��ȡ��ʱ�¼����������ݶ�ȡֹͣ
	if (events & BEV_EVENT_READING && events & BEV_EVENT_READING)
	{
		cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << endl;
		bufferevent_free(be);
		return;
	}
	else if (events & BEV_EVENT_ERROR)
	{
		bufferevent_free(be);
		return;
	}

	//����˵Ĺر��¼�
	if (events & BEV_EVENT_EOF)
	{
		cout << "BEV_EVENT_EOF" << endl;
		bufferevent_free(be);
	}

	//�ͻ������������ӳɹ�
	if (events & BEV_EVENT_CONNECTED)
	{
		cout << "CONNECTED SUCCESSFULL" << endl;
		//�ÿͻ�����������write�¼�
		bufferevent_trigger(be, EV_WRITE, 0);
	}
}

//д�ص�
void client_write_cb(bufferevent *be, void *arg)
{
	cout << "client Write CB" << endl;
	
	//��ȡ�ļ�ָ��
	FILE * fp = (FILE *)arg;
	char readFile_data[1024] = { 0 };
	//��ȡ�ļ������ض�ȡ���ĳ���
	int read_len = fread(readFile_data, 1, sizeof(readFile_data) - 1, fp);
	if (read_len <= 0)
	{
		//������β�����ļ�����
		fclose(fp);	//�ر��ļ�������

		//������̽�be�¼��������ܻ���ɻ�������û�з�����ɶ�����
		//bufferevent_free(be);

		//��д�ص����������ټ���be�¼���д�ص�
		bufferevent_disable(be, EV_WRITE);
		return;
	}
	//д��buffer
	bufferevent_write(be, readFile_data, read_len);
}

//��ȡ�ص�
void client_read_cb(bufferevent *be, void *arg)
{
	cout << "cleint Read CB" << endl;
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
	//���������Ķ���
	event_base * base = event_base_new();

	//����bufferevent�¼�, -1��ʾ�ڲ�����socket
	bufferevent * bevent = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

	//�����˿ں͵�ַ����Զ�̷���˵ĵ�ַ�Ͷ˿�
	sockaddr_in saddr;
	memset(&saddr, 0x0, sizeof saddr);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(5001);
	evutil_inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);

	//��һ���ļ�����ȡ��ָ��
	FILE *fp = fopen("info.txt", "rb");

	//���ûص�����
	bufferevent_setcb(bevent, client_read_cb, client_write_cb, client_event_cb, fp);
	bufferevent_enable(bevent, EV_READ | EV_WRITE);

	//���ӵ��ͻ���
	int ret = bufferevent_socket_connect(bevent, (sockaddr *)&saddr, sizeof saddr);
	if (ret == 0)
	{
		cout << "connected..." << endl;
	}

	//�����¼���ѭ��
	event_base_dispatch(base);
	event_base_free(base);

	return 0;
}