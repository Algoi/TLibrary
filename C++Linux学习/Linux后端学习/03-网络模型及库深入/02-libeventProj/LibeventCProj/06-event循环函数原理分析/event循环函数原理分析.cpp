/*
1�����¼�ѭ������
	int event_base_loop(struct event_base *base,int flags);
	#define EVLOOP_ONCE 0x01				�ȴ�һ���¼����У�ֱ��û�л���¼����˳�
	#define EVLOOP_NONBLOCK 0x02			�л�¼�����û�л�¼������̷���
	#define EVLOOP_NO_EXIT_ON_EMPTY 0x04	û������¼�Ҳ������	event_base_dispatch(base) = event_base_loop(base, 0); ֻҪ���¼���ӵ���event_base�������У���ô��һֱѭ��������¼�û�б���ӵ�base�У���ôֱ���˳�ѭ����*//*	int event_base_loopexit(struct event_base *	base, const struct timeval *tv);
	���������м����¼�(�¼�������)�Ļص�֮���˳��¼�ѭ��û������ʱ����һ�ֻص���ɺ�����ֹͣ
	tv��ʾ��������Ҫ����ָ����ʱ����˳�

	int event_base_loopbreak(struct event_base * base);
	ִ���굱ǰ���ڴ�����¼��������˳����������˳�*/