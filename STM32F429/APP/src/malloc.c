#include "malloc.h"

//�ڴ��(32�ֽڶ���)
//__align(32) uint8_t mem1base[MEM1_MAX_SIZE] __attribute__ ((at(0x20000000)));                                                  //�ڲ�SRAM�ڴ��

__align(32) uint8_t mem1base[MEM1_MAX_SIZE];
//�ڴ�����
uint32_t mem1mapbase[MEM1_ALLOC_TABLE_SIZE];

//�ڴ�������
// const uint32_t memtblsize[SRAMBANK];

//�ڴ���������
struct  _m_mallco_dev mallco_dev =
{
    my_mem_init,
    my_mem_perused,
    mem1base,
    mem1mapbase,
    0
};
/**
 * �����ڴ�
 * *s:�ڴ��׵�ַ
 * c:Ҫ���õ�ֵ
*/
void mymemset(void *s,uint8_t c,uint32_t count)
{
    uint8_t *xs = s;
    while(count--)
    {
        *xs++ =c;
    }
}

/**
 * �ڴ�����ʼ��
 * memx�������ڴ��
*/
void my_mem_init(void)
{
    mymemset(mallco_dev.memmap,0,MEM1_ALLOC_TABLE_SIZE *4);
    mallco_dev.memrdy =1;
}
/**
 * ��ȡ�ڴ�ʹ����
 * 
*/
uint16_t my_mem_perused(void)
{
    uint32_t used =0;
    uint32_t i;
    for(i =0;i<MEM1_ALLOC_TABLE_SIZE;i++)
    {
        if(mallco_dev.memmap[i])
        {
            used++;
        }
    }
    return (used*1000)/(MEM1_ALLOC_TABLE_SIZE);
}

/**
 * �ڴ���䣨�ڲ����ã�
 * 
*/
uint32_t my_mem_malloc(uint32_t size)
{
    signed long offset =0;
    uint32_t nmemb;                                                 //��Ҫ���ڴ����
    uint32_t cmemb =0;                                              //�����ڴ����
    uint32_t i;
    if(!mallco_dev.memrdy)
    {
        mallco_dev.init();
    }
    if(size ==0)
    {
        return 0xFFFFFFFF;                                          //����Ҫ����
    }
    nmemb =size/MEM1_BLOCK_SIZE;
    if(size %MEM1_BLOCK_SIZE)
    {
        nmemb++;
    }
    for(offset =MEM1_ALLOC_TABLE_SIZE-1;offset >=0;offset--)
    {
        if(!mallco_dev.memmap[offset])
        {
            cmemb++;
        }else
        {
            cmemb =0;
        }
        if(cmemb ==nmemb)
        {
            for(i =0;i<nmemb;i++)
            {
                mallco_dev.memmap[offset+i] =nmemb;
            }
            return (offset*MEM1_BLOCK_SIZE);
        }
    }
    return 0xFFFFFFFF;
}

/**
 * �ͷ��ڴ�
*/
uint8_t my_mem_free(uint32_t offset)
{
    int i;
    if(!mallco_dev.memrdy)
    {
        mallco_dev.init();
        return 1;                                   //δ��ʼ��
    }
    if(offset <MEM1_MAX_SIZE)
    {
         int index =offset/MEM1_BLOCK_SIZE;
         int nmemb =mallco_dev.memmap[index];
         for(i =0;i<nmemb;i++)
         {
             mallco_dev.memmap[index+i]=0;
         }
         return 0;
    }else return 2;
}
/**
 * �ͷ��ڴ�(�ⲿ����)
 * 
*/
void myfree(void *ptr)
{
    uint32_t offset;
    if(ptr ==NULL)
    return;
    offset =(uint32_t)ptr -(uint32_t)mallco_dev.membase;
    my_mem_free(offset);
}

/**
 * �����ڴ�(�ⲿ����)
*/
void *mymalloc(uint32_t size)
{
    uint32_t offset;
    offset =my_mem_malloc(size);
    if(offset ==0xFFFFFFFF)
    {
        return NULL;
    }else{
        return (void*)((uint32_t)mallco_dev.membase+offset);
    }
}