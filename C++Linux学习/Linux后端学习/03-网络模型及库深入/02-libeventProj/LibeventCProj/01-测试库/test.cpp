#include <iostream>
#include <event2/event.h>
using namespace std;

/**
	1��������Ŀ���ر���Ŀ��Ԥ����ͷ
	2������ͷ�ļ�����Ŀ���� --> C/C++ --> ���� --> ���Ӱ���Ŀ¼ --> ../../repository/include
			��ʱ��������libevent���ͷ�ļ�
	3��������ļ�����Ŀ���� --> ������ --> ���� --> ���ӿ�Ŀ¼ --> ../../repository/lib
*/

int main()
{
#ifdef _WIN32
	//Windowsƽ̨����Ҫ��ʼ��socket��
	WSADATA wdata;
	WSAStartup(MAKEWORD(2, 2), &wdata);
#endif

	cout << "HELLO LIBEVENT..." << endl;
	event_base * base = event_base_new();
	cout << base << endl;
}