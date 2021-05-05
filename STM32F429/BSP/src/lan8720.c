#include "lan8720.h"

ETH_HandleTypeDef   ETH_Handler;                        //��̫�����
ETH_DMADescTypeDef  *DMARxDscrTab;                      //��̫��DMA�������������ݽṹָ��
ETH_DMADescTypeDef  *DMATxDscrTab;                      //��̫��DMA�������������ݽṹָ�� 
uint8_t *Rx_Buff;                                       //��̫���ײ���������bufferָ��
uint8_t *Tx_Buff;                                       //��̫���ײ���������bufferָ��

//�ر������ж�(���ǲ�����fault��NMI�ж�)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}

uint8_t LAN8720_Init(void)
{      
    uint8_t macaddress[6];
    
    INTX_DISABLE();                         //�ر������жϣ���λ���̲��ܱ���ϣ�
    PCF8574_WriteBit(ETH_RESET_IO,1);       //Ӳ����λ
    delay_ms(100);
    PCF8574_WriteBit(ETH_RESET_IO,0);       //��λ����
    delay_ms(100);
    INTX_ENABLE();                          //���������ж�  

    macaddress[0]=lwipdev.mac[0]; 
	macaddress[1]=lwipdev.mac[1]; 
	macaddress[2]=lwipdev.mac[2];
	macaddress[3]=lwipdev.mac[3];   
	macaddress[4]=lwipdev.mac[4];
	macaddress[5]=lwipdev.mac[5];
        
	ETH_Handler.Instance=ETH;
    ETH_Handler.Init.AutoNegotiation=ETH_AUTONEGOTIATION_ENABLE;//ʹ����Э��ģʽ 
    ETH_Handler.Init.Speed=ETH_SPEED_100M;//�ٶ�100M,�����������Э��ģʽ�������þ���Ч
    ETH_Handler.Init.DuplexMode=ETH_MODE_FULLDUPLEX;//ȫ˫��ģʽ�������������Э��ģʽ�������þ���Ч
    ETH_Handler.Init.PhyAddress=LAN8720_PHY_ADDRESS;//LAN8720��ַ  
    ETH_Handler.Init.MACAddr=macaddress;            //MAC��ַ  
    ETH_Handler.Init.RxMode=ETH_RXINTERRUPT_MODE;   //�жϽ���ģʽ 
    ETH_Handler.Init.ChecksumMode=ETH_CHECKSUM_BY_HARDWARE;//Ӳ��֡У��  
    ETH_Handler.Init.MediaInterface=ETH_MEDIA_INTERFACE_RMII;//RMII�ӿ�  
    if(HAL_ETH_Init(&ETH_Handler)==HAL_OK)
    {
        return 0;   //�ɹ�
    }
    else return 1;  //ʧ��
}


/**
 * ETH�ײ�������ʱ��ʹ�ܣ���������
 * 
*/

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef    GPIO_InitStruct;
    __HAL_RCC_ETH_CLK_ENABLE();                         //����ETHʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();                       //����GPIOAʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();                       //����GPIOBʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();                       //����GPIOCʱ��
    __HAL_RCC_GPIOG_CLK_ENABLE();                       //����GPIOGʱ��

    /**������������RMII�ӿ�
     * ETH_MDIO------------>PA2
     * ETH_MDC------------->PC1
     * ETH_RMII_REF_CLK---->PA1
     * ETH_RMII_CRS_DV----->PA7
     * ETH_RMII_RXD0------->PC4
     * ETH_RMII_RXD1------->PC5
     * ETH_RMII_TX_EN------>PB11
     * ETH_RMII_TXD0------->PG13
     * ETH_RMII_TXD1------->PG14
     * ETH_RESET----------->PCF8274��չIO
    */
   //PA1,2,7
   GPIO_InitStruct.Pin =GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
   GPIO_InitStruct.Mode =GPIO_MODE_AF_PP;                           //���츴��
   GPIO_InitStruct.Pull =GPIO_NOPULL;                               //����������
   GPIO_InitStruct.Speed =GPIO_SPEED_HIGH;                          //����
   GPIO_InitStruct.Alternate =GPIO_AF11_ETH;                        //����ΪETH����
   HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);                           //��ʼ��

   //PB11
   GPIO_InitStruct.Pin =GPIO_PIN_11;
   HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);

   //PC1,4,5
   GPIO_InitStruct.Pin =GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
   HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);                           //��ʼ��

   //PG13,14
   GPIO_InitStruct.Pin =GPIO_PIN_13|GPIO_PIN_14;                    //PG13,14
   HAL_GPIO_Init(GPIOG,&GPIO_InitStruct);                           

   HAL_NVIC_SetPriority(ETH_IRQn,1,0);
   HAL_NVIC_EnableIRQ(ETH_IRQn);

}

/**
 * ��ȡPHY�Ĵ���ֵ
*/
uint32_t LAN8720_ReadPHY(uint16_t reg)
{
    uint32_t regval;
    HAL_ETH_ReadPHYRegister(&ETH_Handler,reg,&regval);
    return regval;
}
/**
 * ��LAN8720ָ���Ĵ���д��ֵ
 * reg:Ҫд��ļĴ���
 * valueLҪд���ֵ
*/
void LAN8720_WritePHY(uint16_t reg,uint16_t value)
{
    uint32_t temp =value;
    HAL_ETH_ReadPHYRegister(&ETH_Handler,reg,&temp);
}

/**
 * �õ�8720 ���ٶ�ģʽ
 * ����ֵ:
 * 001:10M��˫��
 * 101:10Mȫ˫��
 * 010:100M��˫��
*/
uint8_t LAN8720_Get_Speed(void)
{
    uint8_t speed;
    speed =((LAN8720_ReadPHY(31)&0x1C) >>2);
    return speed;
}

//�жϷ�����
void ETH_IRQHandler(void)
{
    while(ETH_GetRxPktSize(ETH_Handler.RxDesc))
    {
        lwip_pkt_hanle();                                                           //������̫�����ݣ����������ύ��LWIP
    }
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_NIS);                            //���DMA�жϱ�־λ
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_R);                             //���DMA�����жϱ�־λ
}


/**
 * ��ȡ���յ���֡����
 * DMARxDesc:����DMA������
 * ����ֵ�����յ���֡����
*/
uint32_t ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDscr)
{
    uint32_t frameLength =0;
    if(((DMARxDscr->Status &ETH_DMARXDESC_OWN) ==(uint32_t) RESET) &&
       ((DMARxDscr->Status &ETH_DMARXDESC_ES) ==(uint32_t) RESET)&&
       ((DMARxDscr->Status &ETH_DMARXDESC_LS) ==(uint32_t) RESET))
       {
           frameLength =((DMARxDscr->Status &ETH_DMARXDESC_FL) >>ETH_DMARXDESC_FRAME_LENGTHSHIFT);
       }
       return frameLength;
}
uint8_t ETH_Mem_Malloc(void)
{
    DMARxDscrTab =mymalloc(ETH_RXBUFNB*sizeof(ETH_DMADescTypeDef));
    DMATxDscrTab =mymalloc(ETH_TXBUFNB*sizeof(ETH_DMADescTypeDef));
    Rx_Buff =mymalloc(ETH_RX_BUF_SIZE*ETH_RXBUFNB);
    Tx_Buff =mymalloc(ETH_TX_BUF_SIZE*ETH_TXBUFNB);
    if(!(uint32_t)&DMARxDscrTab||!(uint32_t)&DMATxDscrTab||!(uint32_t)&Rx_Buff||!(uint32_t)&Tx_Buff)
    {
       ETH_Mem_Free();
       return 1;
    }
    return 0;
}



//�ͷ�ETH �ײ�����������ڴ�
void ETH_Mem_Free(void)
{
    myfree(DMARxDscrTab);               //�ͷ��ڴ�
    myfree(DMATxDscrTab);               //�ͷ��ڴ�
    myfree(Rx_Buff);
    myfree(Tx_Buff);
}
