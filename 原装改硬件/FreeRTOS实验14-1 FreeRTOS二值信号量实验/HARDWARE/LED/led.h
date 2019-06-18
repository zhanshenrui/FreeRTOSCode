#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
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

void LED_Init(void);//初始化

		 				    
#endif
