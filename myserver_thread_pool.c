/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:myserver_thread_pool.c
*   Author  :Lematin
*   Date    :2017-2-17
*   Describe:
*
********************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h>


typedef struct task 
{ 
	void *(*process) (void *arg); 
	void  *arg;
	struct task *next; 
}Cthread_task; 


/*线程池结构*/ 
typedef struct 
{ 
	pthread_mutex_t queue_lock; 
	pthread_cond_t queue_ready; 

	/*链表结构，线程池中所有等待任务*/ 
	Cthread_task *queue_head; 

	/*是否销毁线程池*/ 
	int shutdown; 
	pthread_t *threadid; 

	/*线程池中线程数目*/ 
	int max_thread_num; 

	/*当前等待的任务数*/ 
	int cur_task_size; 

} Cthread_pool; 

static Cthread_pool *pool = NULL; 

void *thread_routine (void *arg); 

void pool_init (int max_thread_num) 
{ 
	int i = 0;

	pool = (Cthread_pool *) malloc (sizeof (Cthread_pool)); 

	pthread_mutex_init (&(pool->queue_lock), NULL); 
	/*初始化条件变量*/
	pthread_cond_init (&(pool->queue_ready), NULL); 

	pool->queue_head = NULL; 

	pool->max_thread_num = max_thread_num; 
	pool->cur_task_size = 0; 

	pool->shutdown = 0; 

	pool->threadid = (pthread_t *) malloc (max_thread_num * sizeof (pthread_t)); 

	for (i = 0; i < max_thread_num; i++) 
	{  
		pthread_create(&(pool->threadid[i]), NULL, thread_routine, NULL); 
	} 
} 



/*向线程池中加入任务*/ 
int pool_add_task (void *(*process) (void *arg), void *arg) 
{ 
	/*构造一个新任务*/ 
	Cthread_task *task = (Cthread_task *) malloc (sizeof (Cthread_task)); 
	task->process = process; 
	task->arg = arg; 
	task->next = NULL;

	pthread_mutex_lock (&(pool->queue_lock)); 
	/*将任务加入到等待队列中*/ 
	Cthread_task *member = pool->queue_head; 
	if (member != NULL) 
	{ 
		while (member->next != NULL) 
		  member = member->next; 
		member->next = task; 
	} 
	else 
	{ 
		pool->queue_head = task; 
	} 

	pool->cur_task_size++; 
	pthread_mutex_unlock (&(pool->queue_lock)); 

	pthread_cond_signal (&(pool->queue_ready)); 

	return 0; 
} 



/*销毁线程池，等待队列中的任务不会再被执行，但是正在运行的线程会一直 
  把任务运行完后再退出*/ 
int pool_destroy () 
{ 
	if (pool->shutdown) 
	  return -1;/*防止两次调用*/ 
	pool->shutdown = 1; 

	/*唤醒所有等待线程，线程池要销毁了*/ 
	pthread_cond_broadcast (&(pool->queue_ready)); 

	/*阻塞等待线程退出，否则就成僵尸了*/ 
	int i; 
	for (i = 0; i < pool->max_thread_num; i++) 
	  pthread_join (pool->threadid[i], NULL); 
	free (pool->threadid); 

	/*销毁等待队列*/ 
	Cthread_task *head = NULL; 
	while (pool->queue_head != NULL) 
	{ 
		head = pool->queue_head; 
		pool->queue_head = pool->queue_head->next; 
		free (head); 
	} 
	/*条件变量和互斥量也别忘了销毁*/ 
	pthread_mutex_destroy(&(pool->queue_lock)); 
	pthread_cond_destroy(&(pool->queue_ready)); 

	free (pool); 
	/*销毁后指针置空是个好习惯*/ 
	pool=NULL; 
	return 0; 
} 


void * thread_routine (void *arg) 
{ 
	//printf ("starting thread 0x%x\n", pthread_self ()); 
	while (1) 
	{ 
		pthread_mutex_lock (&(pool->queue_lock)); 

		while (pool->cur_task_size == 0 && !pool->shutdown) 
		{ 
			//printf ("thread 0x%x is waiting\n", pthread_self ()); 
			pthread_cond_wait (&(pool->queue_ready), &(pool->queue_lock)); 
		} 

		/*线程池要销毁了*/ 
		if (pool->shutdown) 
		{ 
			/*遇到break,continue,return等跳转语句，千万不要忘记先解锁*/ 
			pthread_mutex_unlock (&(pool->queue_lock)); 
			//printf ("thread 0x%x will exit\n", pthread_self ()); 
			pthread_exit (NULL); 
		} 

		// printf ("thread 0x%x is starting to work\n", pthread_self ()); 


		/*待处理任务减1，并取出链表中的头元素*/ 
		pool->cur_task_size--; 
		Cthread_task *task = pool->queue_head; 
		pool->queue_head = task->next; 
		pthread_mutex_unlock (&(pool->queue_lock)); 

		/*调用回调函数，执行任务*/ 
		(*(task->process)) (task->arg); 
		free (task); 
		task = NULL; 
	} 
	/*这一句应该是不可达的*/ 
	pthread_exit (NULL); 
}

