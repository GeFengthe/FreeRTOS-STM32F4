#include "lcd.h"

SRAM_HandleTypeDef  SRAM_Handler;                           //SRAM���
/**
 * SRAM�ײ�����,ʱ��ʹ�ܣ����ŷ���
 * 
*/
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    __HAL_RCC_FMC_CLK_ENABLE();                                         //ʹ��FMCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();                                       //ʹ��GPIODʱ��
    __HAL_RCC_GPIOE_CLK_ENABLE();                                       //ʹ��GPIOEʱ��

    //��ʼ��PD0,1,4,5,7,8,9,10,13,14,15
    GPIO_InitStruct.Pin =GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|\
                         GPIO_PIN_10|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode =GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull =GPIO_PULLUP;
    GPIO_InitStruct.Speed =GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate =GPIO_AF12_FMC;           //����FMC����
    HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);

    //��ʼ��PE7,8,9,10,11,12,13,14,15
    GPIO_InitStruct.Pin =GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|\
                         GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
}

void LCD_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;
    FMC_NORSRAM_TimingTypeDef   FMC_ReadWriteTim;
    FMC_NORSRAM_TimingTypeDef   FMC_WriteTim;

    __HAL_RCC_GPIOB_CLK_ENABLE();                                                   //����GPIOBʱ��
    GPIO_InitStruct.Pin =GPIO_PIN_5;
    GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull =GPIO_PULLUP;
    GPIO_InitStruct.Speed =GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);

    SRAM_Handler.Instance =FMC_NORSRAM_DEVICE;
    SRAM_Handler.Extended =FMC_NORSRAM_EXTENDED_DEVICE;

    SRAM_Handler.Init.NSBank =FMC_NORSRAM_BANK1;                                //ʹ��NE1
    SRAM_Handler.Init.DataAddressMux =FMC_DATA_ADDRESS_MUX_DISABLE;             //��ַ/�����߲�����
    SRAM_Handler.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;                        //SRAM
    SRAM_Handler.Init.MemoryDataWidth =FMC_NORSRAM_MEM_BUS_WIDTH_16;            //16λ���ݿ��
    SRAM_Handler.Init.BurstAccessMode =FMC_BURST_ACCESS_MODE_DISABLE;           //�Ƿ�ʹ��ͻ�����ʣ�����ͬ��ͻ���洢����Ч���˴�δ�õ���

    

}