#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H
#include "main.h"

#define UDP_DEMO_RX_BUFSIZE         2000                    //����udp���������ݳ���
#define UDP_DEMO_PORT               8089                    //����udp���ӵı��ض˿ں�
#define LWIP_SEND_DATA              0x80                    //���������ݷ���

extern uint8_t udp_flag;  
extern void udp_demo_init(void);
#endif