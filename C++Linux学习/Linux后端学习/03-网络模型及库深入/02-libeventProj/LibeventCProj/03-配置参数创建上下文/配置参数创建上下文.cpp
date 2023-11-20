
/*
����������event_base����

1��ͨ��event_base *event_base_new(void)��������Ĭ���������Ե������Ķ���

2��void event_base_free(struct event_base *)
		�ͷ�event_base�ڲ�����Ŀռ估�䱾�����Ŀռ䣬���ͷ��¼���socket���ڻص�����������Ŀռ�

3��event_base *event_base_new_with_config(const struct event_config *);
	ͨ��event_config���������������Ķ���

4��event_reinit int event_reinit(struct event_base *base); ��fork���ӽ��������³�ʼ��������
*/

/*
�����������Զ��� struct event_config

1��event_config *event_config_new(void):				����event_config����
2��void event_config_free(struct event_config *cfg)��	�ͷ�event_config����

//����config����
3��event_config_require_features(struct event_config *cfg, event_method_feature):����config����
		EV_FEATURE_ET = 0x01, ���ش����ĺ��
		EV_FEATURE_O1 = 0x02, Ҫ����ӡ�ɾ�������¼�������ȷ���ĸ��¼�����Ĳ�����O��1�����Ӷȵĺ��
		EV_FEATURE_FDS = 0x04, Ҫ��֧�������ļ��������������������׽��ֵĺ�ˣ�һ���򿪲���ѡ��epollģ�ͣ�
		EV_FEATURE_EARLY_CLOSE = 0x0��
			������ӹر��¼���������ʹ������������Ӻ�ʱ�رգ������ش������ж�ȡ���й�������ݡ��������к�˶�֧��EV_CLOSED��
			������ʹ��EV_CLOSED������ӹرգ�������Ҫ��ȡ���й�������ݡ�
		//windowsϵͳ�£������ĸ�ȫ��Ĭ�ϲ�֧�֣�Linuxϵͳ�£������ĸ�ֻ�е�������֧��

4��event_config_avoid_method(struct event_config *cfg, const char *method);
		ȡ������ģ�ͣ�windows����ֻ��win32��������linux����������select����ģ�ͣ�
		event_config_avoid_method(config, "epoll");
		event_config_avoid_method(config, "poll");

5��event_config_set_flag(struct event_config *cfg, int flag)
		EVENT_BASE_FLAG_NOLOCK : ��Ϊ�����Ķ���event_base���������ڵ��̳߳���������ⲿ���м���������£��������������־��
		EVENT_BASE_FLAG_STARTUP_IOCP : ������Windowsƽ̨������IOCP�¼�����ģʽ�����������IO����
			{
					event_config_set_flag(config, EVENT_BASE_FLAG_STARTUP_IOCP);
					//��ʼ��iocp���߳�
					evthread_use_windows_threads();
					//����cpu����
					SYSTEM_INFO sinfo;
					GetSystemInfo(&sinfo);	//��ȡ�������Ϣ
					event_config_set_num_cpus_hint(config, sinfo.dwNumberOfProcessors);
			}
		EVENT_BASE_FLAG_IGNORE_ENV : ���ں��Ի�������������libevent�����������У���ʹ�û������������ÿ����Ϊ��
		EVENT_BASE_FLAG_NO_CACHE_TIME : �������¼�ѭ��ÿ��׼��ִ�г�ʱ�ص�ʱ��⵱ǰʱ�䣬������ÿ�γ�ʱ�ص�����м�⡣
										ע�⣺������ĸ����CPUʱ��
		EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST : epoll����Ч����ֹͬһ��fd��μ����¼���fd��������ƻ���bug
		EVENT_BASE_FLAG_PRECISE_TIMER : Ĭ��ʹ��ϵͳ���ļ�ʱ���ƣ����ϵͳ�н����Ҹ���ȷ������á�

*/