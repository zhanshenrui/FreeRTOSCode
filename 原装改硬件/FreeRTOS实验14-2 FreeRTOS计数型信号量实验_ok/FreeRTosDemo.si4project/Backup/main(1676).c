/*
xTaskCreate创建任务
vTaskDelete删除任务
vTaskStartScheduler启动系统，开始任务调度
vTaskDelay系统延时，可以让任务切换
delay_ms可以让任务切换的延时，实际是调用vTaskDelay来让任务切换
delay_us，delay_xms不能让任务切换

xTaskCreate第一个参数是任务函数，第二个参数是任务名字，以便调试用，第三个参数是任务堆栈大小，
第四个参数是任务函数的参数列表，第五个参数是任务的优先级，第六个参数是任务句柄
*/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
	KEY_Init();
	EXTIX_Init();
	LCD_Init();
	printf("start\n");
	POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"ATK STM32F103/407");	
	LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 6-3");
	LCD_ShowString(30,50,200,16,16,"Task Susp and Resum");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2016/11/25");

	xTaskCreate(start_task,
		"start_task",
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		StartTask_Handler
		);
	vTaskStartScheduler();
}

void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();

	
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}

