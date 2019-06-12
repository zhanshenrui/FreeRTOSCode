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
    BEEP_Init();
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
    Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(u8));
    Message_Queue=xQueueCreate(MESSAGE_Q_NUM,USART_REC_LEN);
	xTaskCreate(task1_task,
		"task1_task",
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		Task1Task_Handler
		);
	xTaskCreate(Keyprocess_task,
		"Keyprocess_task",
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		Keyprocess_Handler
		);
	
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}
//LED0������ 
void task1_task(void *pvParameters)
{
    u8 key,i=0;
    BaseType_t err;
    while(1)
    {
        key=KEY_Scan(0);
        if((Key_Queue!=NULL)&&(key))
        {
            err=xQueueSend(Key_Queue, &key, 10);
            
            if(err==errQUEUE_FULL)
                printf("key_queue is full\r\n");
        }
        i++;
        if(i%10==0)
            check_msg_queue();
        if(i>50)
        {
            i=0;
            LED0=!LED0;
        }
        vTaskDelay(10);
    }
}   

//LED1������
void Keyprocess_task(void *pvParameters)
{
    u8 num,getkey;
    while(1)
    {
        if(Key_Queue)
        {
            if(xQueueReceive(Key_Queue, getkey, portMAX_DELAY))
            {
                switch(getkey)
                {
                    case WAKEUP_PRES:
                        LED1=!LED1;
                        break;
                    case KEY2_PRES:
                        BEEP=!BEEP;
                        break;
                    cade KEY0_PRES;
                        num++;
                        printf("num++");
                }
            }
        }
        vTaskDelay(10);
    }
}


