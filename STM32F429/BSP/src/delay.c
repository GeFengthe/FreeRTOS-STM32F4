#include "delay.h"

void delay_us(uint32_t nus)
{
	uint32_t tickcnt,ticksrt,tickend;
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