#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f4xx.h"

//���������ڴ��
#define  SRAMIN                                 0                               //�ڲ��ڴ��
#define  SRAMEX                                 1                               //�ⲿ�ڴ��
#define  SRAMCCM                                2                               //CCM�ڴ�أ��˲��ֽ�CPU���Է���)

#define  SRAMBANK                               3                               //����֧�ֵ�SRAM��


#define  MEM1_BLOCK_SIZE                        64                           //�ڴ���СΪ64�ֽ�
#define  MEM1_MAX_SIZE                          100*1024                    //������0x2000000 -0x20020000  128k ,0x20020000���滹��64k ��0x1000 0000ӳ���64k ֻ��cpu����
#define  MEM1_ALLOC_TABLE_SIZE                  MEM1_MAX_SIZE/MEM1_BLOCK_SIZE  //�ڴ���С 2048

//�ڴ���������
struct _m_mallco_dev
{
    void (*init) (void);                                         //��ʼ��
    uint16_t (*perused)(void);                                  //�ڴ�ʹ����
    uint8_t *membase;                                               //�ڴ�� ����SRAMBANK��������ڴ�
    uint32_t *memmap;                                             //�ڴ����״̬��
    uint8_t memrdy;                                                 //�ڴ�����Ƿ����
};




void mymemset(void *s,uint8_t c,uint32_t count);	                 //�����ڴ�
void mymemcpy(void *des,void *src,uint32_t n);                      //�����ڴ�     
void my_mem_init(void);				                         //�ڴ�����ʼ������(��/�ڲ�����)
uint32_t my_mem_malloc(uint32_t size);	                //�ڴ����(�ڲ�����)
uint8_t my_mem_free(uint32_t offset);		            //�ڴ��ͷ�(�ڲ�����)
uint16_t my_mem_perused(void) ;			                    //����ڴ�ʹ����(��/�ڲ�����) 


void myfree(void *ptr);
void *mymalloc(uint32_t size);

#endif

