#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


//#define USER2 PDin(3)   	//PE4
//#define USER1 PAin(8)	//PE3 
//#define TAMPER PCin(13)	//PE2
//#define WAKEUP PAin(0)	//PA0  WK_UP


#define USER2  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)//读取按键3
#define USER1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)//读取按键2
#define TAMPER  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)//读取按键1
#define WAKEUP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键0(WK_UP) 

#define KEY_0	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY_1	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)
#define KEY_2	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)
#define KEY_ENTER	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)




 

#define USER2_PRES 		4//KEY0按下
#define USER1_PRES		3	//KEY1按下
#define TAMPER_PRES		2	//KEY2按下
#define WAKEUP_PRES   	1	//KEY_UP按下(即WK_UP/KEY_UP)

#define KEY0_PRES 1
#define KEY1_PRES 2
#define KEY2_PRES 3
#define KEYENTER_PRES 4


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数					    
#endif
