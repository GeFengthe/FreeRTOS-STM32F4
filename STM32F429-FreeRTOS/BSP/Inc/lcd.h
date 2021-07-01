#ifndef __LCD_H
#define __LCD_H
#include "stm32f4xx.h"
#include "main.h"

//*--------------------LCD ���ָ��---------------------
#define     LCD_READID                          0x3D            //������ID
#define     LCD_WCOLUMNADDR                     0x2A            //д���е�ַ
#define     LCD_WPAGEADDR                        0x2B            //д��ҳ��ַ
#define     LCD_WGRAMADDR                        0x2C            //д��GRAM��ַ
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
#define	LCD_LED PBout(5) 	//LCD����	PB5	    
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


#endif