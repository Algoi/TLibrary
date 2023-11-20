//简易版线程池
#include "threadpoolsimple.h"

//全局的线程池的对象，且该对象唯一
ThreadPool *thrPool = NULL;

//任务开始编号
int beginnum = 1000;

/**
	创建（初始化）线程池
	指定线程池中的子线程的个数，以及任务队列能够存放的最大的任务数
*/
void create_threadpool(int thrnum,int maxtasknum)
{
    //printf("begin call %s-----\n",__FUNCTION__);
	
	//初始化全局的线程池对象，开辟在堆空间
    thrPool = (ThreadPool*)malloc(sizeof(ThreadPool));

	//确定线程池内的子线程数
    thrPool->thr_num = thrnum;
	//确定任务池内最大的任务数
    thrPool->max_job_num = maxtasknum;
	
    thrPool->shutdown = 0;//是否摧毁线程池，1代表摧毁
    thrPool->job_push = 0;//任务队列添加的位置
    thrPool->job_pop = 0;//任务队列出队的位置
    thrPool->job_num = 0;//初始化的任务个数为0

    thrPool->tasks = (PoolTask*)malloc((sizeof(PoolTask)*maxtasknum));//申请最大的任务队列

    //初始化锁和条件变量
    pthread_mutex_init(&thrPool->pool_lock,NULL);
    pthread_cond_init(&thrPool->empty_task,NULL);
    pthread_cond_init(&thrPool->not_empty_task,NULL);

    int i = 0;
    thrPool->threads = (pthread_t *)malloc(sizeof(pthread_t)*thrnum);//申请n个线程id的空间
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    for(i = 0;i < thrnum;i++)
	{
        pthread_create(&thrPool->threads[i],&attr,thrRun,(void*)thrPool);//创建多个线程
    }
    //printf("end call %s-----\n",__FUNCTION__);
}

/**
	摧毁线程池
*/
void destroy_threadpool(ThreadPool *pool)
{
    pool->shutdown = 1;//开始自爆
    pthread_cond_broadcast(&pool->not_empty_task);//诱杀 

    int i = 0;
    for(i = 0; i < pool->thr_num ; i++)
	{
        pthread_join(pool->threads[i],NULL);
    }

	//销毁锁和条件变量
    pthread_cond_destroy(&pool->not_empty_task);
    pthread_cond_destroy(&pool->empty_task);
    pthread_mutex_destroy(&pool->pool_lock);

	//释放内存
    free(pool->tasks);
    free(pool->threads);
    free(pool);
}

/**
	添加任务到线程池
*/
void addtask(ThreadPool *pool)
{
    //printf("begin call %s-----\n",__FUNCTION__);
    pthread_mutex_lock(&pool->pool_lock);

	//实际任务总数大于最大任务个数则阻塞等待(等待任务被处理)
    while(pool->max_job_num <= pool->job_num)
	{
        pthread_cond_wait(&pool->empty_task,&pool->pool_lock);
    }

    int taskpos = (pool->job_push++)%pool->max_job_num;
    //printf("add task %d  tasknum===%d\n",taskpos,beginnum);
    pool->tasks[taskpos].tasknum = beginnum++;
    pool->tasks[taskpos].arg = (void*)&pool->tasks[taskpos];
    pool->tasks[taskpos].task_func = taskRun;
    pool->job_num++;

    pthread_mutex_unlock(&pool->pool_lock);

    pthread_cond_signal(&pool->not_empty_task);//通知包身工
    //printf("end call %s-----\n",__FUNCTION__);
}

/**
	子线程的任务函数，取任务、执行回调函数
*/
void *thrRun(void *arg)
{
    //printf("begin call %s-----\n",__FUNCTION__);
    ThreadPool *pool = (ThreadPool*)arg;
    int taskpos = 0;//任务位置
    PoolTask *task = (PoolTask *)malloc(sizeof(PoolTask));

    while(1)
	{
        //获取任务，先要尝试加锁
        pthread_mutex_lock(&thrPool->pool_lock);

		//无任务并且线程池不是被销毁的状态
        while(thrPool->job_num <= 0 && !thrPool->shutdown )
		{
			//如果没有任务，线程会阻塞
            pthread_cond_wait(&thrPool->not_empty_task,&thrPool->pool_lock);
        }
        
        if(thrPool->job_num)
		{
            //有任务需要处理
            taskpos = (thrPool->job_pop++)%thrPool->max_job_num;
            //printf("task out %d...tasknum===%d tid=%lu\n",taskpos,thrPool->tasks[taskpos].tasknum,pthread_self());
			//为什么要拷贝？避免任务被修改，生产者会添加任务，因为执行任务回调函数在最后那里
            memcpy(task,&thrPool->tasks[taskpos],sizeof(PoolTask));
            task->arg = task;
            thrPool->job_num--;
            //task = &thrPool->tasks[taskpos];
            pthread_cond_signal(&thrPool->empty_task);//通知生产者
        }

        if(thrPool->shutdown)
		{
            //代表要摧毁线程池，此时线程退出即可
            //pthread_detach(pthread_self());//临死前分家
            pthread_mutex_unlock(&thrPool->pool_lock);
            free(task);
			pthread_exit(NULL);
        }

        //释放锁
        pthread_mutex_unlock(&thrPool->pool_lock);
		//执行回调函数
        task->task_func(task->arg);
    }
    
    //printf("end call %s-----\n",__FUNCTION__);
}

//任务回调函数
void taskRun(void *arg)
{
    PoolTask *task = (PoolTask*)arg;
    int num = task->tasknum;
    printf("task %d is runing %lu\n",num,pthread_self());

    sleep(1);
    printf("task %d is done %lu\n",num,pthread_self());
}


int main()
{
    create_threadpool(3,20);
    int i = 0;
    for(i = 0;i < 50 ; i++)
	{
        addtask(thrPool);//模拟添加任务
    }

    sleep(20);
    destroy_threadpool(thrPool);

    return 0;
}
