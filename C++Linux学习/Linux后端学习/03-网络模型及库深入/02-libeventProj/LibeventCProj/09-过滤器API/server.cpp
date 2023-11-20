#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <string.h>
#include <event2/event_struct.h>
#include <event2/bufferevent_struct.h>
#include <event2/buffer.h>
#include <ctype.h>
#ifndef _WIN32
#include <signal.h>
#else
#endif
#include <string>
using namespace std;

//�¼��ص�
void event_cb(bufferevent *be, short events, void *arg)
{
	cout << "client Event CB" << endl;
}

//д�ص�
void write_cb(bufferevent *be, void *arg)
{
	cout << "Write CB" << endl;
}

//���ص�
void read_cb(bufferevent *be, void *arg)
{
	cout << "Read CB" << endl;

	//��ȡ���Թ��˵�����
	char data[1024] = { 0 };
	bufferevent_read(be, data, sizeof(data) - 1);
	cout << data << endl;

	//��Ӧ����
	char rdata[1024] = "<h1>Hello World</h1>";
	bufferevent_write(be, rdata, strlen(rdata));
}

//���������
enum bufferevent_filter_result filter_input_cb(
	struct evbuffer *src, struct evbuffer *dst, ev_ssize_t dst_limit,
	enum bufferevent_flush_mode mode, void *ctx)
{
	cout << "Input CB" << endl;

	char data[1024];
	//�ӻ������ж�ȡ����
	int readLen = evbuffer_remove(src, data, sizeof(data) - 1);

	//����ĸת�ɴ�д
	for (int i = 0; i < sizeof(data) - 1; i++)
	{
		data[i] = toupper(data[i]);
	}

	//������֮�������д��Ŀ�껺�����������¼���������
	evbuffer_add(dst, data, readLen);

	return BEV_OK;
}

//���������
enum bufferevent_filter_result filter_output_cb(
	struct evbuffer *src, struct evbuffer *dst, ev_ssize_t dst_limit,
	enum bufferevent_flush_mode mode, void *ctx)
{
	cout << "Output CB" << endl;

	//������������ݣ��������httpЭ��ͷ
	string str = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nConten-Length: ";

	char data[1024] = { 0 };
	int readLen = evbuffer_remove(src, data, sizeof(data) - 1);

	str += std::to_string(readLen);
	str += "\r\n\r\n";
	str += data;

	evbuffer_add(dst, str.data(), str.size());

	return BEV_OK;
}

//���ӻص�����
void listen_cb(struct evconnlistener *, evutil_socket_t fd, struct sockaddr * addr, int socklen, void * arg)
{
	cout << "listen CB" << endl;

	//�����Ķ���
	event_base * base = (event_base *)arg;

	//����bufferevent�����Ķ��� BEV_OPT_CLOSE_ON_FREE����buffereventʱ�ر�socket
	bufferevent * bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	//����event������
	bufferevent * bev_filter = bufferevent_filter_new(bev,
		filter_input_cb,		//����������ص�����
		filter_output_cb,		//����������ص�����
		BEV_OPT_CLOSE_ON_FREE,	//���ͷ�bev_filter�����ʱ���ͷ���֮������bev����
		NULL,
		NULL
	);

	//��Ӽ���¼�
	bufferevent_enable(bev_filter, EV_READ | EV_WRITE);

	//���ûص�����
	bufferevent_setcb(bev_filter, read_cb, write_cb, event_cb, base);
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

	//�������������
	//�趨�����Ķ˿ں͵�ַ
	sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(5001);

	//���Ӽ�����
	evconnlistener * listener = evconnlistener_new_bind(
		base,
		listen_cb,
		base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,
		(sockaddr *)&saddr,
		sizeof saddr
	);

	//�����¼���ѭ��
	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_base_free(base);

	return 0;
}