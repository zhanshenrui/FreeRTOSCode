/*
xTaskCreate��������
vTaskDeleteɾ������
vTaskStartScheduler����ϵͳ����ʼ�������
vTaskDelayϵͳ��ʱ�������������л�
delay_ms�����������л�����ʱ��ʵ���ǵ���vTaskDelay���������л�
delay_us��delay_xms�����������л�

xTaskCreate��һ�����������������ڶ����������������֣��Ա�����ã������������������ջ��С��
���ĸ��������������Ĳ����б��������������������ȼ���������������������

delay_xms�������������л�������Ϊ��ʱ��Ƭ�л��������������л�����
configTICK_RATE_HZ=20
printf("task1 run:%d\r\n",task1_num);��ӡǰ�������˹ؼ��Σ����������л��Դ�ӡ��Ӱ��
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
//LED0������ 
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

//LED1������
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


