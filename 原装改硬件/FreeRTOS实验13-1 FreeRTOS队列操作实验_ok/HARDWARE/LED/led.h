#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define DS1 PFout(6)// PF6
#define DS2 PFout(7)// PF7	
#define DS3 PFout(8)// PF8
#define DS4 PFout(9)// PF9

#define LED0 DS1
#define LED1 DS2
#define LED2 DS3
#define LED3 DS4

#define ON 0
#define OFF 1

void LED_Init(void);//��ʼ��

		 				    
#endif
