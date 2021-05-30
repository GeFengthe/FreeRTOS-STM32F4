#include "lwip_tcp_demo.h"
#include "lwip/api.h"
#include "string.h"
#include "mqtt.h"
#include "delay.h"

#define MQTT_CONNECT_ACK                4
#define MQTT_DISCONNECT_ACK             2
#define MQTT_HEART_ACK                  2
#define MQTT_SUBTOPIC_ACK               2

typedef enum{
    MQTT_CONNECT_START=1,
    MQTT_CONNECT_SUCCESS,
    MQTT_PUBLISHDATA,
    MQTT_SUB_TOPIC,    
}MQTT_CONNECT_STATE;

struct netconn *tcp_clientconn;                 //TCP_CLIENT���ӽṹ��



u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];                         //TCP�ͻ��˽������ݻ�����
u8 *tcp_client_sendbuf;                         //TCP�ͻ��˷������ݻ�����
u32 tcp_len;
u8 mqtt_alive;                                  //�Ƿ�������MQTT��־
u8 subtopic_flag;                    


//���ӳɹ���������Ӧ 20 02 00 00
//�ͻ��������Ͽ����� e0 00

const u8 parket_connetAck[] = {0x20,0x02,0x00,0x00};
const u8 parket_disconnect[] = {0xe0,0x00};
const u8 parket_heart[] = {0xc0,0x00};
const u8 parket_heart_reply[] ={0xc0,0x00};
const u8 parket_subAck[] ={0x90,0x03};

//TCP�ͻ����������ȼ�
#define SKY_TCPCLIENT_PRIO                  11

//�����ջ��С
#define SKY_TCPCLIENT_STK_SIZE              512

//TCP�ͻ���������
TaskHandle_t Sky_TcpClientThread_Handler;             //TCP�ͻ����߳�


/**
*@beif 
*/

void mqtt_sendheart(void)
{
    lwip_tcpClient_send((u8 *)parket_heart,2);
}


void lwip_tcpClient_send(u8 * sendbuf,u32 length)
{
    err_t err;
    err=netconn_write(tcp_clientconn,sendbuf,length,NETCONN_COPY);
    if(err !=ERR_OK)
    {
      printf("����ʧ��\r\n");
    }
}
/***
*@beif  mqtt_MessageProcess
*@para  mag,length,type
*@data  2021-5-28
*@auth  GECHENG
*/
static void mqtt_MessageProcess(u8 *mag,u32 length)
{
    if(length ==MQTT_CONNECT_ACK)
    {
        if(mag[0] ==parket_connetAck[0]&& mag[1] ==parket_connetAck[1]&&mag[2] ==parket_connetAck[3])
        {
            mqtt_alive =1;                                                                              //MQTT���������߱�־
        }
    }else if(length ==MQTT_DISCONNECT_ACK)
    {
        if(mag[0] ==parket_disconnect[0]&&mag[1] ==parket_disconnect[1])
        {
            mqtt_alive =0;
        }
    }else if(length ==MQTT_SUBTOPIC_ACK)
    {
        if(mag[0] ==parket_subAck[0]&&mag[1] ==parket_subAck[1])
        {
            subtopic_flag =1;                                                                           //���ⶩ�ĳɹ���־
        }
    }
}

/***
*@beif  Sky_TcpClientThread
*@para  void
*@data  2021-5-30
*@auth  GECHENG
*@note  MQTT�ͻ����̣߳����ȼ�Ϊ10
*/
static void Sky_TcpClientThread(void *arg)
{
    u32 data_len =0;
    struct pbuf *q;
    static ip_addr_t server_ipaddr,loca_ipaddr;
    static u16_t server_port,loca_port;
    err_t err,recv_err;
    server_port=REMOTE_PORT;                                                                                                        //Զ�˶˿ں�
    IP4_ADDR(&server_ipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);                       //����ʼ���ṹ��lwipdev��Զ������IP��ַת��
    mqtt_alive =0;                                                                                      //��ʼ��MQTT�ͻ���
    tcp_clientconn =netconn_new(NETCONN_TCP);                                                           //����һ��TCP����
    err =netconn_connect(tcp_clientconn,&server_ipaddr,server_port);                                    //���ӷ�����    
    if(err !=ERR_OK)
    {
        netconn_delete(tcp_clientconn);
        printf("���ӷ�����ʧ��\r\n");
    }else if(err==ERR_OK)
    {
        struct netbuf *recvbuf;                                                                         //���ջ�����
        tcp_clientconn->recv_timeout =10;    
        netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1);
        printf("�����Ϸ�����%d.%d.%d.%d,���ض˿ں�Ϊ:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
        tcp_client_sendbuf =pvPortMalloc(200);
        
        MQTT_Connect_Pack(tcp_client_sendbuf,&tcp_len);
        printf("tcp_len =%d\r\n",tcp_len);
        lwip_tcpClient_send(tcp_client_sendbuf,tcp_len);
        
        vPortFree(tcp_client_sendbuf);
        printf("err=%d\r\n",err);
        while(1)
        {       
            if((recv_err =netconn_recv(tcp_clientconn,&recvbuf))==ERR_OK)
            {
                taskENTER_CRITICAL();
                    
                memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);
                for(q=recvbuf->p;q!=NULL;q=q->next)                                                                         //����TCP���������
                {
                    if(q->len >(TCP_CLIENT_RX_BUFSIZE-data_len))
                    {
                        memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));
                            
                    }else
                    {
                        memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
                    }
                        data_len+=q->len;
                        if(data_len>TCP_CLIENT_RX_BUFSIZE)
                        {
                            data_len =TCP_CLIENT_RX_BUFSIZE;
                            break;
                        }
                }
                mqtt_MessageProcess(tcp_client_recvbuf,data_len);
                data_len =0;
                taskEXIT_CRITICAL();
                
                printf("%s\r\n",tcp_client_recvbuf);
                netbuf_delete(recvbuf);
             }else if(recv_err ==ERR_CLSD)
             {
                 netconn_close(tcp_clientconn);
                 netconn_delete(tcp_clientconn);
                 printf("������%d.%d.%d.%d�Ͽ�����\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
                 break;
             }else if(recv_err ==ERR_TIMEOUT)
             {
                 printf(" tcp task TIMEOUT\r\n");
             }
                vTaskDelay(10);
         }
            
    }
        
}


/***
*@beif  Sky_TcpClientThread_Init
*@para  void
*@data  2021-5-30
*@auth  GECHENG
*@note  TCP��ΪMQTT�ͻ����̳߳�ʼ�������ȼ�Ϊ10
*/
void Sky_TcpClientThread_Init(void)
{
    BaseType_t err;
    err =xTaskCreate((TaskFunction_t )Sky_TcpClientThread,
                     (const char *)"tcp_cient",
                     (uint32_t ) SKY_TCPCLIENT_STK_SIZE,
                     (void *) NULL,
                     (UBaseType_t ) SKY_TCPCLIENT_PRIO,
                     (TaskHandle_t *)&Sky_TcpClientThread_Handler);
    if(err !=pdPASS)
    {
        printf("Tcp Client Thread Fail\r\n");
    }
}

