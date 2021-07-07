#ifndef __LCD_H
#define __LCD_H
#include "stm32f4xx.h"
#include "main.h"

//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
//#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)


//*--------------------LCD ���ָ��---------------------
#define     LCD_READID                          0xD3            //������ID
#define     LCD_WCOLUMNADDR                     0x2A            //д���е�ַ
#define     LCD_WPAGEADDR                       0x2B            //д��ҳ��ַ
#define     LCD_WGRAMADDR                       0x2C            //д��GRAM��ַ
#define     LCD_MADCLADDR                       0x36            //�洢������ָ�������Ļ����ˢ�·�ʽ
//------------------------------------------------------
//LCD��Ҫ������
typedef struct  
{		 	 
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;				//LCD ID
	u8  dir;			//���������������ƣ�0��������1��������	
	u16	wramcmd;		//��ʼдgramָ��
	u16 setxcmd;		//����x����ָ��
	u16 setycmd;		//����y����ָ�� 
}sky_lcd;

extern sky_lcd skylcd;              //����LCD��Ҫ����
//////////////////////////////////////////////////////////////////////////////////	 
//-----------------MCU�� LCD�˿ڶ���---------------- 
#define	LCD_LED(n)          n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);             	//LCD����	PB5	    
//LCD��ַ�ṹ��
typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;
//ʹ��NOR/SRAM�� Bank1.sector1,��ַλHADDR[27,26]=00 A18��Ϊ�������������� 
//ע������ʱSTM32�ڲ�������һλ����!  			    
#define LCD_BASE        ((u32)(0x60000000 | 0x0007FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
//////////////////////////////////////////////////////////////////////////////////

typedef enum{
    LCD_VERTICAL=0,
    LCD_LANDSCAPE
}LCD_DIR;

typedef enum{
    L2R_U2D=0,                      //������,���ϵ���
    L2R_D2U,                        //������,���µ���
    R2L_U2D,                        //���ҵ���,���ϵ���
    R2L_D2U,                        //���ҵ���,���µ���
    U2D_L2R,                        //���ϵ���,������
    U2D_R2L,                        //���ϵ���,���ҵ���
    D2U_L2R,                        //���µ���,������
    D2U_R2L                         //���µ���,���ҵ���
}SCAN_DIR;

#define DEFAULT_SCAN_DIR            L2R_U2D             //Ĭ��ɨ�跽��


extern void LCD_Init(void);
extern void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
#endif