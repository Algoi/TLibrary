#include <iostream>
#include <event2/event.h>
using namespace std;

/**
	1、创建项目，关闭项目的预编译头
	2、引入头文件：项目属性 --> C/C++ --> 常规 --> 附加包含目录 --> ../../repository/include
			此时可以引入libevent库的头文件
	3、引入库文件：项目属性 --> 链接器 --> 常规 --> 附加库目录 --> ../../repository/lib
*/

int main()
{
#ifdef _WIN32
	//Windows平台下需要初始化socket库
	WSADATA wdata;
	WSAStartup(MAKEWORD(2, 2), &wdata);
#endif

	cout << "HELLO LIBEVENT..." << endl;
	event_base * base = event_base_new();
	cout << base << endl;
}