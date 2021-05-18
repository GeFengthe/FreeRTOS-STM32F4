#include "delay.h"

//ʹ��FreeRTOSʱ�õ� �δ�ʱ�������xPortSysTickHandler����
extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{  
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//ϵͳ�Ѿ�����
    {
        xPortSysTickHandler();	
    }
    HAL_IncTick();
}

void delay_us(uint32_t nus)
{
	uint32_t tickcnt=0,ticksrt,tickend;
	uint32_t reload = SysTick->LOAD;                                         //��ȡװ��ֵ
    uint32_t ticktotal =nus *180;                                           //�ܽ�����
    ticksrt =SysTick->VAL;
    while(1)
    {
        tickend =SysTick->VAL;
        if(ticksrt >tickend)
            tickcnt +=ticksrt -tickend;
        else
            tickcnt +=reload -tickend+ticksrt;
        ticksrt =tickend;
        if(tickcnt >=ticktotal)
            break;     
    }	
}


void delay_ms(uint16_t nms)
{
    while(nms--)
    {
        delay_us(1000);
    }
}
