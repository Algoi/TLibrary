/********************************************************************************************************************************/
/**
 * @brief
 *      1����һ���µ� bufferevent��������������� underlying ��
 *      2�������������������� bufferevent �������Ա���������������ʱ������Ӧ�Ĵ���
 *      3��options ���Կ��� bufferevent ����Ϊ���������� / ���ó�ʱ�¼���ˮλ�ߡ��¼�֪ͨ��
 *
 * @param underlying        �󶨵�bufferevent����
 * @param input_filter      ������˻ص����������ڶ�ȡ�ص�����ִ��
 * @param output_filter     ������˻ص�����
 * @param options           BEV_OPT_CLOSE_ON_FREE���ͷŹ�����ʱ�ͷ���֮������bufferevent����
 * @param free_context      ָ�����ͷŹ�����ʱ�����õĺ��������ͷŹ�����������κ������ڴ�
 * @param ctx               ָ�򽫴��ݸ� free_context �����������Ķ���
 * @return struct bufferevent*  ���ع���������
 */
 /*
 struct bufferevent *
	 bufferevent_filter_new(
		 struct bufferevent *underlying,
		 bufferevent_filter_cb input_filter,
		 bufferevent_filter_cb output_filter,
		 int options,
		 void(*free_context)(void *),
		 void *ctx);
 */

 /**
  * @brief   ������˻ص������ڶ�ȡ�ص����������ͬ��
  *
  * @param src           Դ���ݵ�ַ
  * @param dst           Ŀ�����ݵ�ַ�������ڴ���Դ����֮��������ݵĵ�ַ
  * @param dst_limit     һ����ʾĿ�껺�����Ĵ�С���Ƶ�����ֵ
  *                      dst_limit �����ڻ������������Ļص������б�ʾĿ�껺�����Ĵ�С���ơ�
  *                      ���δָ�������ƣ���Ĭ��Ϊ -1����ʾû�д�С���ơ�����ζ����Ĭ������£�Ŀ�껺�����Ĵ�С���Ը�����Ҫ��̬����������������д���������ݡ�
  * @param mode          һ����ʾ������ˢ��ģʽ��ö��ֵ������ָ����ʱӦ�ôӻ������ж�ȡ��д������
  *                      �� mode ��ֵΪ BEV_NORMAL ʱ���ص����������ȡ�����ܶ�����ݣ�������д�뵽Ŀ�껺�����С�
  *                      ���Դ�������е����ݱ�Ŀ�껺��������ֻ��Ŀ�껺�����������µ����ݲŻᱻд��Ŀ�껺�����С����Ŀ�껺�����Ĵ�С����Ϊ 0���򲻻�д���κ����ݡ�
  *                      �� mode ��ֵΪ BEV_FLUSH ʱ���ص�����Ӧ�þ����ܿ�ؽ���������д�뵽Ŀ�껺�����У��������Ƿ��Ѿ��ﵽ��Ŀ�껺�����Ĵ�С���ơ�
  *                      �� mode ��ֵΪ BEV_FINISHED ʱ���ص�����Ӧ������κα�Ҫ�����������ͷ�������Դ��
  * @param ctx           һ��ָ�������Ķ����ָ�룬�ö��󽫱����ݸ��ص�����
  * @return
  *                      BEV_OK�������������ɹ�������������д洢�˲��ֻ�ȫ�����������ݡ�
						 BEV_NEED_MORE����Ҫ������������ݲ�����ɹ�����������
						 BEV_ERROR������������ʧ�ܣ������˴���
  */
  /*
  enum bufferevent_filter_result bufferevent_filter_cb(
	  struct evbuffer *src,
	  struct evbuffer *dst,
	  ev_ssize_t dst_limit,
	  enum bufferevent_flush_mode mode,
	  void *ctx);
  */

  /********************************************************************************************************************************/
  /**
   * @brief ���ڴ� evbuffer ���������Ƴ����ݣ��������ݴ洢�� data_out ��
   *
   * @param buf
   * @param data_out
   * @param datlen
   */
   //int evbuffer_remove(struct evbuffer *buf, void *data_out, size_t datlen);

   /**
	* @brief ������ָ���Ļ����� buf ���������
	*
	* @param buf
	* @param data_in
	* @param datlen
	* @return int  ��������ֵ��һ�� int ���͵���������ʾ�ɹ��򻺳�����ӵ��ֽ������������ֵС�� datlen�����ʾ�������ʧ�ܡ�
	*/
	//int evbuffer_add(struct evbuffer *buf, const void *data_in, size_t datlen);

	/********************************************************************************************************************************/


	/**
	 * @brief ��evbuffer_remove������ͬ��peek at data inside an evbuffer without removing it or copying it out.
	 *
	 * @param buffer: Ҫ�鿴���ݵ�evbuffer������
	 * @param len: Ҫ�鿴�ĳ���
	 * @param start_at: evbuffer_ptr�ṹ�����͵�ָ��,��ʾ���ĸ�λ�ÿ�ʼ�鿴���ݡ���ΪNULL,���buffer�������Ŀ�ͷ��ʼ�鿴���ݡ�
	 * @param vec_out: evbuffer_iovec�ṹ�����͵�ָ��,���ڽ������ݡ�
	 * @param n_vec: Ҫ���յ����ݿ�������

	 * @return �ú�������һ������ֵ,��ʾʵ�ʻ�ȡ�����ݳ���,������صĳ���С��ָ���ĳ���,��˵����������û���㹻�����ݿɹ��鿴

	 ע�⣺
		evbuffer_ptr �ṹ�����ͱ�ʾ�����������ݵ�λ����Ϣ���ڵ��øú���ǰ��Ҫ�ȳ�ʼ����
		evbuffer_iovec �ṹ�����ͱ�ʾ��Ҫ���յ�������Ϣ���ڵ��øú���ǰ��Ҫ�ȷ����ڴ�ռ䡣
		�ú��������ƶ��������е�����λ�ã���˶�ε��øú��������ظ���ȡ��ͬ�����ݡ�
	 */
	 /*
	 int evbuffer_peek(
		 struct evbuffer *buffer,		//Ҫ�鿴���ݵ�evbuffer������
		 ev_ssize_t len,					//Ҫ�鿴���ݵĳ���
		 struct evbuffer_ptr *start_at,	//��ʾ���ĸ�λ�ÿ�ʼ�鿴���ݣ�ΪNULL���ͷ��ʼ
		 struct evbuffer_iovec *vec_out, //���ڽ�������
		 int n_vec);						//Ҫ���ܵ����ݿ������
	 */


	 /**
	  * @brief �����������ǽ���������ǰ len ���ֽ�ɾ�������������������ǰ�ƶ������������������ܴ�С�ᷢ���仯�����������ڲ����洢�����ݲ���ı䡣
	  *
	  * @param buf  ��ʾҪ�����Ļ�����
	  * @param len  ��ʾҪɾ�����ֽ���
	  * @return int ����һ������ֵ����ʾʵ��ɾ�����ֽ����������ִ�й����г��ִ����򷵻�-1��

	 ��Ҫע����ǣ�evbuffer_drain ����ֻɾ���˻������е����ݣ���û���ͷŻ�����ռ�õ��ڴ�ռ䡣�����Ҫ�ͷ��ڴ棬����Ҫ���� evbuffer_free �������ͷŻ�����ռ�õ��ڴ�ռ䡣
	 ���⣬evbuffer_drain ������Ч�ʽϵͣ�����Ҫ�������������ǰ�ƶ������Ҳ��ܱ�֤ÿ�ζ��ܹ��ɹ�ɾ��ָ�����ȵ����ݡ���ˣ���ʵ��ʹ���У���Ҫ����ѡ��ú������������������һ��ʹ�ã��Դﵽ���ߵ�Ч�ʺͿɿ��ԡ�
	  */
	  //int evbuffer_drain(struct evbuffer *buf, size_t len);


	  /**
	   * @brief	��Ϊ������ buf Ԥ�� size �ֽڵĿռ䣬�������ܹ��洢���ݵ������ṹ�������������
	   *			������˵���ú����Ὣ size ��ֳ����ɸ���С������ EV_SSIZE_MAX �Ŀ飬Ȼ���ŵ������ṹ������ vec �С�
	   *
	   * @param buf	ָ����Ҫ����Ԥ���ռ������ evbuffer ��������
	   * @param size	��ҪԤ���Ŀռ��С��
	   * @param vec	���ڴ�����ݿ�������ṹ�����顣
	   * @param n_vec	�����ṹ������ vec ��������С,������ܴ�� n_vec �������ṹ�塣�� size ���� n_vec �������ṹ�����ܴ�ŵ�����ʱ,�����᷵�ش���
	   * @return int	�����ṹ������ vec ��ʵ�ʴ�ŵ������ṹ���������������ֵС�ڵ��� 0�����ʾ��������ʧ�ܡ�

	   ��Ҫע����ǣ��ú���ֻ��Ϊ������Ԥ���ռ䣬�����������򻺳���д�����ݡ�Ҫ�򻺳���д�����ݣ���Ҫʹ��evbuffer_add()ϵ�к�����
	   */
	   //int evbuffer_reserve_space(struct evbuffer *buf, ev_ssize_t size, struct evbuffer_iovec *vec, int n_vec);

	   /********************************************************************************************************************************/