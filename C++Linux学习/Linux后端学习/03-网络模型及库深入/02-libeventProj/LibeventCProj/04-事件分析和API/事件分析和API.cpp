/*

1���¼�״̬����:
	1.1 �ѳ�ʼ����initialized�� ����event_new֮��
	1.2 ������pending�� ����event_add֮�� ֻ�д�״̬�Ż�����¼��ص�
	1.3 ���active�� �¼�������������ʱ�¼�
	1.4 �־õģ�persistent�� 
		���û���ó־�ÿ���¼��������ú�״̬�ͱ�Ϊ�Ǵ�����Ҳ���ղ����¼��ˣ���Ҫ�ٴε���event_add��

2���¼�API����
	2.1 struct event *event_new(struct event_base *base, evutil_socket_t fd, short what, event_callback_fn cb, void *arg);
		���ܣ���ʼ��event�¼�����ͨ������ֵ���ء�ʵ���¼��ĳ�ʼ��״̬
		������
			base�������Ľڵ�
			fd�������ļ�������
			what���¼���־
				EV_TIMEOUT	��ʱ�¼���Ĭ���Ǻ��Եģ�ֻ��������¼�ʱ���ó�ʱ�¼�����Ч
				EV_READ		���¼�
				EV_WRITE	д�¼�
				EV_SIGNAL	�ź��¼�
				EV_PERSIST	�־á����ǲ��ӣ���ô�¼�����һ�δ���֮��Ͳ��ٴ�������Ҫ�ٴ���Ӹ��¼����ܴ���
				EV_ET		��Ե��������Ҫ�ײ�֧�֣�2.0����֧�֣�Ӱ��EV_READ��EV_WRITE
			cb���ص��������¼�����ʱִ�еĺ�����ԭ�ͣ�void(*event_callback_fn)(evutil_socket_t fd, short what, void *arg);
				fd�������¼����ļ�������
				what���¼���־
				arg�����ݹ����Ĳ�����ͬevent_new���������һ������
			arg�����ص��������ݵĲ���
	2.2 int event_add(struct event *ev, const struct timeval *tv)
		���ܣ�����¼����˺��¼�����תΪ����״̬���ɹ�����0��ʧ�ܷ���-1
		������
			ev����Ҫ��ӵ��¼�����
			tv����ʱʱ�䣬����ֱ�Ӵ�NULL������ʱ��ʱ��Ҫ���þ����
	2.3 int event_del(struct event *ev);
		���ܣ�ɾ������ָ�����¼�����ֻ�ǽ��¼�״̬����˷Ǵ���״̬������ͨ��event_add�ٴ���ӱ�Ϊ����״̬
	2.4 void event_free(struct event *event)
		���ܣ��Ѵ����ͼ���״̬���ã�����ռ�ǰ��ʹ��״̬��Ϊδ�����ͷǼ���
	2.5 int event_pending(const struct event *ev, short events, struct timeval *tv);
		���ܣ��ж�ָ�����¼��Ƿ��ڴ���״̬
*/

/*
�����ź��¼�API��
	#define evsignal_add(ev, tv)			event_add((ev), (tv))
	#define evsignal_new(b, x, cb, arg)		event_new((b), (x), EV_SIGNAL|EV_PERSIST, (cb), (arg))
	#define evsignal_del(ev)				event_del(ev)
	#define evsignal_pending(ev, tv)		event_pending((ev), EV_SIGNAL, (tv))
*/

/*
���ڶ�ʱ���¼�API��
	#define evtimer_new(b, cb, arg)		event_new((b), -1, 0, (cb), (arg))
			0����ʾ�����κε��¼���־
	#define evtimer_add(ev, tv)			event_add((ev), (tv))
	#define evtimer_del(ev)				event_del(ev)
	#define evtimer_pending(ev, tv)		event_pending((ev), EV_TIMEOUT, (tv))

	��ʱ�����Ż�
	const struct timeval *event_base_init_common_timeout(
						struct event_base *base,
						const struct timeval *duration);
*/