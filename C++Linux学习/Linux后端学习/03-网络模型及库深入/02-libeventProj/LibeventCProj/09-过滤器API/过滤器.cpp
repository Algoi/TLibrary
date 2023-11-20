/********************************************************************************************************************************/
/**
 * @brief
 *      1、建一个新的 bufferevent，并将其与给定的 underlying 绑定
 *      2、将输入和输出过滤器与 bufferevent 关联，以便在数据输入和输出时进行相应的处理
 *      3、options 可以控制 bufferevent 的行为，例如启用 / 禁用超时事件、水位线、事件通知等
 *
 * @param underlying        绑定的bufferevent对象
 * @param input_filter      输入过滤回调函数，先于读取回调函数执行
 * @param output_filter     输出过滤回调函数
 * @param options           BEV_OPT_CLOSE_ON_FREE（释放过滤器时释放与之关联的bufferevent对象）
 * @param free_context      指向在释放过滤器时将调用的函数，以释放过滤器分配的任何其他内存
 * @param ctx               指向将传递给 free_context 函数的上下文对象
 * @return struct bufferevent*  返回过滤器对象
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
  * @brief   输入过滤回调，先于读取回调函数（输出同）
  *
  * @param src           源数据地址
  * @param dst           目标数据地址，用于在处理源数据之后输出数据的地址
  * @param dst_limit     一个表示目标缓冲区的大小限制的整数值
  *                      dst_limit 参数在缓冲区过滤器的回调函数中表示目标缓冲区的大小限制。
  *                      如果未指定该限制，则默认为 -1，表示没有大小限制。这意味着在默认情况下，目标缓冲区的大小可以根据需要动态增长，以容纳所有写入它的数据。
  * @param mode          一个表示缓冲区刷新模式的枚举值，用于指定何时应该从缓冲区中读取或写入数据
  *                      当 mode 的值为 BEV_NORMAL 时，回调函数将会读取尽可能多的数据，并将其写入到目标缓冲区中。
  *                      如果源缓冲区中的数据比目标缓冲区大，则只有目标缓冲区能容纳下的数据才会被写入目标缓冲区中。如果目标缓冲区的大小限制为 0，则不会写入任何数据。
  *                      当 mode 的值为 BEV_FLUSH 时，回调函数应该尽可能快地将所有数据写入到目标缓冲区中，而不管是否已经达到了目标缓冲区的大小限制。
  *                      当 mode 的值为 BEV_FINISHED 时，回调函数应该完成任何必要的清理工作并释放所有资源。
  * @param ctx           一个指向上下文对象的指针，该对象将被传递给回调函数
  * @return
  *                      BEV_OK：过滤器操作成功，输出缓冲区中存储了部分或全部的输入数据。
						 BEV_NEED_MORE：需要更多的输入数据才能完成过滤器操作。
						 BEV_ERROR：过滤器操作失败，出现了错误。
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
   * @brief 用于从 evbuffer 缓冲区中移除数据，并将数据存储到 data_out 中
   *
   * @param buf
   * @param data_out
   * @param datlen
   */
   //int evbuffer_remove(struct evbuffer *buf, void *data_out, size_t datlen);

   /**
	* @brief 用于向指定的缓冲区 buf 中添加数据
	*
	* @param buf
	* @param data_in
	* @param datlen
	* @return int  函数返回值是一个 int 类型的整数，表示成功向缓冲区添加的字节数。如果返回值小于 datlen，则表示添加数据失败。
	*/
	//int evbuffer_add(struct evbuffer *buf, const void *data_in, size_t datlen);

	/********************************************************************************************************************************/


	/**
	 * @brief 与evbuffer_remove函数不同，peek at data inside an evbuffer without removing it or copying it out.
	 *
	 * @param buffer: 要查看数据的evbuffer缓冲区
	 * @param len: 要查看的长度
	 * @param start_at: evbuffer_ptr结构体类型的指针,表示从哪个位置开始查看数据。若为NULL,则从buffer缓冲区的开头开始查看数据。
	 * @param vec_out: evbuffer_iovec结构体类型的指针,用于接收数据。
	 * @param n_vec: 要接收的数据块数量。

	 * @return 该函数返回一个整数值,表示实际获取的数据长度,如果返回的长度小于指定的长度,则说明缓冲区中没有足够的数据可供查看

	 注意：
		evbuffer_ptr 结构体类型表示缓冲区中数据的位置信息，在调用该函数前需要先初始化。
		evbuffer_iovec 结构体类型表示需要接收的数据信息，在调用该函数前需要先分配内存空间。
		该函数不会移动缓冲区中的数据位置，因此多次调用该函数可以重复获取相同的数据。
	 */
	 /*
	 int evbuffer_peek(
		 struct evbuffer *buffer,		//要查看数据的evbuffer缓冲区
		 ev_ssize_t len,					//要查看数据的长度
		 struct evbuffer_ptr *start_at,	//表示从哪个位置开始查看数据，为NULL则从头开始
		 struct evbuffer_iovec *vec_out, //用于接受数据
		 int n_vec);						//要接受的数据块的数量
	 */


	 /**
	  * @brief 函数的作用是将缓冲区中前 len 个字节删除，并将后面的数据向前移动。这样，缓冲区的总大小会发生变化，而缓冲区内部所存储的数据不会改变。
	  *
	  * @param buf  表示要操作的缓冲区
	  * @param len  表示要删除的字节数
	  * @return int 返回一个整数值，表示实际删除的字节数。如果在执行过程中出现错误，则返回-1。

	 需要注意的是，evbuffer_drain 函数只删除了缓冲区中的数据，并没有释放缓冲区占用的内存空间。如果需要释放内存，则需要调用 evbuffer_free 函数来释放缓冲区占用的内存空间。
	 另外，evbuffer_drain 函数的效率较低，它需要将后面的数据向前移动，并且不能保证每次都能够成功删除指定长度的数据。因此，在实际使用中，需要谨慎选择该函数，并结合其他操作一起使用，以达到更高的效率和可靠性。
	  */
	  //int evbuffer_drain(struct evbuffer *buf, size_t len);


	  /**
	   * @brief	是为缓冲区 buf 预留 size 字节的空间，并返回能够存储数据的向量结构体数组的数量。
	   *			具体来说，该函数会将 size 拆分成若干个大小不超过 EV_SSIZE_MAX 的块，然后存放到向量结构体数组 vec 中。
	   *
	   * @param buf	指向需要进行预留空间操作的 evbuffer 缓冲区。
	   * @param size	需要预留的空间大小。
	   * @param vec	用于存放数据块的向量结构体数组。
	   * @param n_vec	向量结构体数组 vec 的容量大小,即最多能存放 n_vec 个向量结构体。当 size 大于 n_vec 个向量结构体所能存放的数据时,函数会返回错误。
	   * @return int	向量结构体数组 vec 中实际存放的向量结构体数量，如果返回值小于等于 0，则表示函数调用失败。

	   需要注意的是，该函数只是为缓冲区预留空间，不会真正地向缓冲区写入数据。要向缓冲区写入数据，需要使用evbuffer_add()系列函数。
	   */
	   //int evbuffer_reserve_space(struct evbuffer *buf, ev_ssize_t size, struct evbuffer_iovec *vec, int n_vec);

	   /********************************************************************************************************************************/