/********************************************************
*   Copyright (C) 2016 All rights reserved.
*   
*   Filename:myserver_net.c
*   Author  :Lematin
*   Date    :2017-2-17
*   Describe:
*
********************************************************/
#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int server_net_init(int *socket_fd)
{
	//1.创建套接字
	if(0>(*socket_fd = socket(AF_INET,SOCK_STREAM,0))){
		perror("socket create fail!");
		return -1;
	}
#ifdef DEBUG	
	printf("socket ... ok!\n");
#endif	
	int on = 1;
	if(0>setsockopt(*socket_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))){
		perror("setsockopt reuseaddr fail!");
		return -1;
	}

	//2.绑定本机地址和端口
	struct sockaddr_in myserver_addr;
	memset(&myserver_addr,0,sizeof(myserver_addr));
	myserver_addr.sin_family		= AF_INET;
	myserver_addr.sin_port			= htons(8888);
	myserver_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	
	if(0>bind(*socket_fd,(struct sockaddr *)&myserver_addr,sizeof(myserver_addr))){
		perror("bind fail!");
		return -1;
	}
#ifdef DEBUG	
	printf("binding ... ok!\n");
#endif	

	//3.设置监听套接字
	if(0>listen(*socket_fd,10)){
		perror("listen fail!");
		return -1;
	}
#ifdef DEBUG	
	printf("listening ...\n");
#endif	
	return 0;

}
