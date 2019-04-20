#ifndef __MYSERVER_THREAD_POOL_H
#define __MYSERVER_THREAD_POOL_H
void pool_init (int max_thread_num);
int pool_add_task (void *(*process) (void *arg), void *arg);
int pool_destroy ();   
#endif
