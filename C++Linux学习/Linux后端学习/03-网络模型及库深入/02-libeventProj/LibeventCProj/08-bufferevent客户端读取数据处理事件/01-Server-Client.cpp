///*/*******************************************************************************
//**                                                                            **
//**                     Jiedi(China nanjing)Ltd.                               **
//**	               ������������ �Ĳܿ����˴��������Ϊѧϰ�ο�                **
//*******************************************************************************/
//
///*****************************FILE INFOMATION***********************************
//**
//** Project       : Libevent C�����߲���������
//** Contact       : xiacaojun@qq.com
//**  ����   : http://blog.csdn.net/jiedichina
//**	��Ƶ�γ� : �����ƿ���	http://study.163.com/u/xiacaojun
//			   ��Ѷ����		https://jiedi.ke.qq.com/
//			   csdnѧԺ		http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961
//**             51ctoѧԺ	http://edu.51cto.com/lecturer/index/user_id-12016059.html
//** 			   ���Ŀ���		http://www.laoxiaketang.com
//**
//**  Libevent C�����߲��������� �γ�Ⱥ ��1003847950 ����Ⱥ���ش���ͽ���
//**   ΢�Ź��ں�  : jiedi2007
//**		ͷ����	 : �Ĳܿ�
//**
//*****************************************************************************
////������������������ Libevent C�����߲��������� �γ�  QQȺ��1003847950 ���ش���ͽ���*/
//#include <iostream>
//#include <event2/event.h>
//#include <event2/listener.h>
//#include <event2/bufferevent.h>
//#include <string.h>
//#ifndef _WIN32
//#include <signal.h>
//#else
//#endif
//#include <string>
//using namespace std;
//
//static string recvstr = "";
//static int recvCount = 0;
//static int sendCount = 0;
//
////���󣬳�ʱ �����ӶϿ�����룩
//void event_cb(bufferevent *be, short events, void *arg)
//{
//	cout << "[E]" << flush;
//	//��ȡ��ʱʱ�䷢�������ݶ�ȡֹͣ
//	if (events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
//	{
//		//��ȡ����������
//		char data[1024] = { 0 };
//		//��ȡ���뻺�����ݣ���Ϊ������ˮλ�����ܻ������ݲ����ڻ�����
//		int len = bufferevent_read(be, data, sizeof(data) - 1);
//		if (len > 0)
//		{
//			recvCount += len;
//			recvstr += data;
//		}
//
//		cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << endl;
//		//bufferevent_enable(be,EV_READ);
//		bufferevent_free(be);
//		cout << recvstr << endl;
//		cout << "recvCount=" << recvCount << " sendCount=" << sendCount << endl;
//	}
//	else if (events & BEV_EVENT_ERROR)
//	{
//		bufferevent_free(be);
//	}
//	else
//	{
//		cout << "OTHERS" << endl;
//	}
//}
//void write_cb(bufferevent *be, void *arg)
//{
//	cout << "[W]" << flush;
//}
//void read_cb(bufferevent *be, void *arg)
//{
//	cout << "[R]" << flush;
//	char data[1024] = { 0 };
//	//��ȡ���뻺������
//	int len = bufferevent_read(be, data, sizeof(data) - 1);
//	//cout << data << flush;
//	if (len <= 0)return;
//	recvstr += data;
//	recvCount += len;
//
//	//�������� д�뵽�������
//	bufferevent_write(be, "OK", 3);
//}
//
//void listen_cb(evconnlistener *ev, evutil_socket_t s, sockaddr*sin, int slen, void *arg)
//{
//	cout << "listen_cb" << endl;
//	event_base *base = (event_base *)arg;
//
//	//����bufferevent������ BEV_OPT_CLOSE_ON_FREE����buffereventʱ�ر�socket
//	bufferevent *bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);
//
//	//��Ӽ���¼�
//	bufferevent_enable(bev, EV_READ | EV_WRITE);
//
//
//	//����ˮλ
//	//��ȡˮλ
//	bufferevent_setwatermark(bev, EV_READ,
//		5,	//��ˮλ 0���������� Ĭ����0
//		10	//��ˮλ 0���������� Ĭ����0
//	);
//
//	bufferevent_setwatermark(bev, EV_WRITE,
//		5,	//��ˮλ 0���������� Ĭ����0 �������ݵ���5 д��ص�������
//		0	//��ˮλ��Ч
//	);
//
//	//��ʱʱ�������
//	timeval t1 = { 0,500000 };
//	bufferevent_set_timeouts(bev, &t1, 0);
//
//	//���ûص�����
//	bufferevent_setcb(bev, read_cb, write_cb, event_cb, base);
//}
//
//
////���󣬳�ʱ �����ӶϿ�����룩
//void client_event_cb(bufferevent *be, short events, void *arg)
//{
//	cout << "[client_E]" << flush;
//	//��ȡ��ʱ�¼����������ݶ�ȡֹͣ
//	if (events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
//	{
//		cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << endl;
//		//bufferevent_enable(be,EV_READ);
//		bufferevent_free(be);
//		return;
//	}
//	else if (events & BEV_EVENT_ERROR)
//	{
//		bufferevent_free(be);
//		return;
//	}
//	//����˵Ĺر��¼�
//	if (events & BEV_EVENT_EOF)
//	{
//		cout << "BEV_EVENT_EOF" << endl;
//		bufferevent_free(be);
//	}
//	//�ͻ������������ӳɹ�
//	if (events & BEV_EVENT_CONNECTED)
//	{
//		cout << "BEV_EVENT_CONNECTED" << endl;
//		//����write�¼�
//		bufferevent_trigger(be, EV_WRITE, 0);
//	}
//}
//void client_write_cb(bufferevent *be, void *arg)
//{
//	cout << "[client_W]" << flush;
//	//��ȡ�ļ�ָ��
//	FILE *fp = (FILE *)arg;
//	char data[1024] = { 0 };
//	int len = fread(data, 1, sizeof(data) - 1, fp);
//	if (len <= 0)
//	{
//		//������β�����ļ�����
//		fclose(fp);
//		//�����������ܻ���ɻ�������û�з��ͽ���
//		//bufferevent_free(be);
//		bufferevent_disable(be, EV_WRITE);
//		return;
//	}
//	sendCount += len;
//	//д��buffer
//	bufferevent_write(be, data, len);
//}
//void client_read_cb(bufferevent *be, void *arg)
//{
//	cout << "[client_R]" << flush;
//}
//
//int main(int argc, char *argv[])
//{
//
//#ifdef _WIN32 
//	//��ʼ��socket��
//	WSADATA wsa;
//	WSAStartup(MAKEWORD(2, 2), &wsa);
//#else
//	//���Թܵ��źţ��������ݸ��ѹرյ�socket
//	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
//		return 1;
//#endif
//	//�����Ķ���
//	event_base *base = event_base_new();
//
//	//�������������
//	//�趨�����Ķ˿ں͵�ַ
//	sockaddr_in sin;
//	memset(&sin, 0, sizeof(sin));
//	sin.sin_family = AF_INET;
//	sin.sin_port = htons(5001);
//
//	//���Ӽ�����
//	evconnlistener *ev = evconnlistener_new_bind(
//		base,
//		listen_cb,		//�ص�����
//		base,			//�ص������Ĳ���arg
//		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
//		10,				//listen back
//		(sockaddr*)&sin,
//		sizeof(sin)
//	);
//
//
//	//�ͻ��˴���
//	{
//		//-1����ʾ�ڲ�����socket
//		bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
//
//		//����˵�ַ�Ͷ˿�
//		sockaddr_in sremoter;
//		memset(&sremoter, 0, sizeof(sin));
//		sremoter.sin_family = AF_INET;
//		sremoter.sin_port = htons(5001);
//		evutil_inet_pton(AF_INET, "127.0.0.1", &sremoter.sin_addr.s_addr);
//
//		//��һ���ļ�����ȡ���ļ�ָ��
//		FILE *fp = fopen("info.txt", "rb");
//
//		//���ûص�����
//		bufferevent_setcb(bev, client_read_cb, client_write_cb, client_event_cb, fp);
//		bufferevent_enable(bev, EV_READ | EV_WRITE);
//
//		//���ӵ������
//		int re = bufferevent_socket_connect(bev, (sockaddr*)&sremoter, sizeof(sremoter));
//		if (re == 0)
//		{
//			cout << "connected" << endl;
//		}
//	}
//
//
//	//�����¼���ѭ��
//	event_base_dispatch(base);
//
//	evconnlistener_free(ev);
//	event_base_free(base);
//
//	return 0;
//}