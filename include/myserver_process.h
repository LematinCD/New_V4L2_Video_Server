#ifndef __MYSERVER_PROCESS_H
#define __MYSERVER_PROCESS_H




void send_video_data_handle(void *args);
//void uartdata_send_handle(void *args);
//void uartdata_recv_handle(void *args,char flag);
//void login_regster_changepwd_handle(int *conn_fd,char *recv_buf,char flag,char *send_buf);
int thread_send_alarm_init();
void *my_net_process(void *args);
#endif
