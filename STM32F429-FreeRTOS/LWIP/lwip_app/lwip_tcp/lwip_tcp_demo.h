#ifndef __LWIP_TCP_DEMO_H
#define __LWIP_TCP_DEMO_H
#include "main.h"
#include "lwip_comm.h"

#define TCP_CLIENT_RX_BUFSIZE               1000        //���ջ�����
#define REMOTE_PORT                         1883        //����Զ������IP��ַ
#define LWIP_SEND_DATA                      0X80        //���������ݷ���

#define FALSE                               1
#define TRUE                                0

extern u8 mqtt_alive;

extern void mqtt_sendheart(void);
extern uint8_t tcp_client_flag; 
extern void Sky_TcpClientThread_Init(void);
extern void lwip_tcpClient_send(u8 * sendbuf,u32 length);
#endif

