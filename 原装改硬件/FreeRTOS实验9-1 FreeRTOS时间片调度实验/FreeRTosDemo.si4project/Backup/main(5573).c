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
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		LED0Task_Handler
		);
	xTaskCreate(led1_task,
		"led1_task",
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		LED1Task_Handler
		);
	
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}
//LED0������ 
void led0_task(void *pvParameters)
{
    while(1)
    {
        LED2=~LED2;
        vTaskDelay(2000);
    }
}   

//LED1������
void led1_task(void *pvParameters)
{
    while(1)
    {
        LED3=0;
        vTaskDelay(200);
        LED3=1;
        vTaskDelay(800);
    }
}


