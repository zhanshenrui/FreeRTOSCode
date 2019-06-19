#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#define TSK_SIZE_128	128
#define TSK_SIZE_50	50

#define TSK_PRIO_1	1
#define TSK_PRIO_2	2
#define TSK_PRIO_3	3

TaskHandle_t StartTask_Handler;
TaskHandle_t LED0Task_Handler;
TaskHandle_t LED1Task_Handler;

void start_task(void *pvParameters);
void led0_task(void *pvParameters);
void led1_task(void *pvParameters);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
	printf("start\n");

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
	xTaskCreate(led0_task,
		"led0_task",
		TSK_SIZE_50,
		NULL,
		TSK_PRIO_2,
		LED0Task_Handler
		);

	xTaskCreate(led1_task,
		"led1_task",
		TSK_SIZE_50,
		NULL,
		TSK_PRIO_3,
		LED1Task_Handler
		);
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}

void led0_task(void *pvParameters)
{
	while(1)
	{
		LED0=~LED0;
		vTaskDelay(1000);
	}
}

void led1_task(void *pvParameters)
{
	while(1)
	{
		LED1=1;
		vTaskDelay(800);
		LED1=0;
		vTaskDelay(200);
	}
}
