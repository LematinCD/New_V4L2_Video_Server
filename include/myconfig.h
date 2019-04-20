#ifndef __CONFIG_H
#define __CONFIG_H

/*摄像头设置*/
#define CAM_NAME       "/dev/video0"
#define MY_WIDE         160     //图片宽度	
#define MY_HIGH         120     //图片高度
#define MY_QUALITY      100     //压缩质量

#ifdef  JPEG
#define MY_CAMERA_FMAT		V4L2_PIX_FMT_MJPEG //摄像头采集格式jpeg
#elif	YUYV
#define MY_CAMERA_FMAT		V4L2_PIX_FMT_YUYV   //摄像头采集格式yuyv
#endif

/*串口设置*/
#define MY_UART_BAUD    115200  //波特率
#define MY_UART_NBITS   8       //数据位
#define MY_UART_NEVENT  'N'     //校验
#define MY_UART_NSTOP   1       //流控



#ifdef M0
#define MY_UART_BUF_SIZE 36     //串口数据大小
#define MY_UART_HEAD2    0xbb    //串口帧头2 --华清的板子
#define MY_UART_HEAD3    0xdd    //串口帧头3 --华清的板子

#elif MY_ZIGBEE
#define MY_UART_BUF_SIZE 64     //串口数据大小
#define MY_UART_HEAD1    0x24    //串口帧头1 --自己的板子
#define MY_UART_TAIL     0x2A    //串口帧尾

#endif



#define MY_UART_BUF_SIZE_POS 5  //串口大小位置
#define MY_UART_BUF_DATA_POS 9  //串口数据位置


/*网络TCP设置*/
#define MY_NET_CMD_POS 4
#define MY_NET_DATA_SIZE 40000







#endif
