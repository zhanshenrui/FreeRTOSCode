#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


//#define USER2 PDin(3)   	//PE4
//#define USER1 PAin(8)	//PE3 
//#define TAMPER PCin(13)	//PE2
//#define WAKEUP PAin(0)	//PA0  WK_UP


#define USER2  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)//��ȡ����3
#define USER1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)//��ȡ����2
#define TAMPER  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)//��ȡ����1
#define WAKEUP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����0(WK_UP) 

#define KEY_0	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY_1	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)
#define KEY_2	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)
#define KEY_ENTER	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)




 

#define USER2_PRES 		4//KEY0����
#define USER1_PRES		3	//KEY1����
#define TAMPER_PRES		2	//KEY2����
#define WAKEUP_PRES   	1	//KEY_UP����(��WK_UP/KEY_UP)

#define KEY0_PRES 1
#define KEY1_PRES 2
#define KEY2_PRES 3
#define KEYENTER_PRES 4


void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��					    
#endif
