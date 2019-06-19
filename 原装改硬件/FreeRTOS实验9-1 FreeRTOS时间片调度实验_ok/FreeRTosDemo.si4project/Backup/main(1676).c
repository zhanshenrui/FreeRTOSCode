/*
xTaskCreate��������
vTaskDeleteɾ������
vTaskStartScheduler����ϵͳ����ʼ�������
vTaskDelayϵͳ��ʱ�������������л�
delay_ms�����������л�����ʱ��ʵ���ǵ���vTaskDelay���������л�
delay_us��delay_xms�����������л�

xTaskCreate��һ�����������������ڶ����������������֣��Ա�����ã������������������ջ��С��
���ĸ��������������Ĳ����б��������������������ȼ���������������������
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(115200);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
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

