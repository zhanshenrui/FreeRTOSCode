/*
xTaskCreate创建任务
vTaskDelete删除任务
vTaskStartScheduler启动系统，开始任务调度
vTaskDelay系统延时，可以让任务切换
delay_ms可以让任务切换的延时，实际是调用vTaskDelay来让任务切换
delay_us，delay_xms不能让任务切换

xTaskCreate第一个参数是任务函数，第二个参数是任务名字，以便调试用，第三个参数是任务堆栈大小，
第四个参数是任务函数的参数列表，第五个参数是任务的优先级，第六个参数是任务句柄

delay_xms不会引起任务切换，但因为有时间片切换，最后还是完成了切换功能
configTICK_RATE_HZ=20
printf("task1 run:%d\r\n",task1_num);打印前都进入了关键段，避免任务切换对打印有影响
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
	xTaskCreate(task1_task,
		"task1_task",
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		Task1Task_Handler
		);
	xTaskCreate(task2_task,
		"task2_task",
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		Task2Task_Handler
		);
	
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}
//LED0任务函数 
void task1_task(void *pvParameters)
{
    u8 task1_num=0;
    while(1)
    {
        task1_num++;
        LED0=!LED0;
        taskENTER_CRITICAL();
        printf("task1 run:%d\r\n",task1_num);
        taskEXIT_CRITICAL();
        delay_xms(10);
    }
}   

//LED1任务函数
void task2_task(void *pvParameters)
{
    u8 task2_num=0;
    while(1)
    {
        task2_num++;
        LED1=!LED1;
        printf("task2 run:%d\r\n",task2_num);
        taskEXIT_CRITICAL();
        delay_xms(10);
    }
}


