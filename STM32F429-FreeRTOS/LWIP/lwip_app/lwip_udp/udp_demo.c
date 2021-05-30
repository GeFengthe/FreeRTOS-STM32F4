#include "udp_demo.h"
#include "lwip_comm.h"
#include "lwip/api.h"
#include "string.h"

//udp�ͻ�������
#define UDP_PRIO            10
//�����ջ��С
#define UDP_STK_SIZE        512
//������ƿ�
TaskHandle_t udp_thread;

u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];

const u8 *udp_demo_sendbuf="SKY world STM32F4/F7 NETCONN UDP demo send data\r\n";

u8 udp_flag;                                                //udp ���ݷ��ͱ�־

static void Sky_udpthread(void *parameter)
{
    static struct netconn *udpconn;                         //udp���ӽṹ��
    static struct netbuf *recvbuf;                          //��������
    static struct netbuf *sentbuf;                          //��������
    struct ip_addr destipaddr;
    struct pbuf *q;
    u32 data_len =0;
    err_t err;
    
    udpconn =netconn_new(NETCONN_UDP);                      //����һ��udp����
    udpconn->recv_timeout =10;                              //netconn_recv()�������߳�,�������ó�ʱʱ��Ϊ10ms���Ͳ��������̣߳�����Ҫʹ��recv_timeout �ֶ�Ҫ��lwipopts.h�ж���LWIP_SO_RECVTIMEO
    
    if(udpconn !=NULL)
    {
        err =netconn_bind(udpconn,IP_ADDR_ANY,UDP_DEMO_PORT);           //��udpconn���ӽṹ�뱾��IP��ַ�Ͷ˿ںŰ�
        IP4_ADDR(&destipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);      //����Ŀ��IP�ĵ�ַ
        netconn_connect(udpconn,&destipaddr,UDP_DEMO_PORT);                     //����Զ������
        
        if(err ==ERR_OK)
        {
            while(1)
            {
                if((udp_flag &LWIP_SEND_DATA)==LWIP_SEND_DATA)
                {
                    
                    sentbuf =netbuf_new();
                    netbuf_alloc(sentbuf,strlen((char *)udp_demo_sendbuf));
                    MEMCPY(sentbuf->p->payload,(void *)udp_demo_sendbuf,strlen((char *)udp_demo_sendbuf)); 
                    err =netconn_send(udpconn,sentbuf);
                    
                    if(err !=ERR_OK)
                    {
                        printf("����ʧ��\r\n");
                        netbuf_delete(sentbuf);
                    }
                    udp_flag &= ~LWIP_SEND_DATA;
                    netbuf_delete(sentbuf);
                }
                
                netconn_recv(udpconn,&recvbuf);                     //��������
                
                if(recvbuf !=NULL)
                {
                    taskENTER_CRITICAL();                           //�����ٽ�Σ��ر��ж�
                    memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);
                    
                    for(q =recvbuf->p;q!=NULL;q =q->next)
                    {
                        if(q->len >(UDP_DEMO_RX_BUFSIZE -data_len))
                        {
                            memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));                //��������
                        }else{
                            memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
                        }
                        data_len +=q->len;
                        if(data_len >UDP_DEMO_RX_BUFSIZE)
                            break;     
                    }
                    taskEXIT_CRITICAL();
                    data_len =0;
                    printf("%s \r\n",udp_demo_recvbuf);
                    netbuf_delete(recvbuf);
                    
                }
                vTaskDelay(5);              //�ӳ�5ms
            }
        }else{
            printf("UDP ��ʧ��\r\n");
        }
        
    }else{
        printf("UDP ���Ӵ���ʧ��\r\n");
    }
}


/*����UDP�߳�*/
void udp_demo_init(void)
{
    UBaseType_t res;
    res =xTaskCreate((TaskFunction_t )Sky_udpthread,
                     (const char *) "udpdemo",
                     (uint16_t )    UDP_STK_SIZE,
                     (void *) NULL,
                     (UBaseType_t ) UDP_PRIO,
                     (TaskHandle_t *)&udp_thread);
    if(res ==NULL)
    {
        printf("ude thread fail\r\n");
    }
//    vTaskStartScheduler();    
}