#ifndef __MAIN_H
#define __MAIN_H

#define TSK_SIZE_128	128
#define TSK_SIZE_50		50

#define TSK_PRIO_1	1
#define TSK_PRIO_2	2
#define TSK_PRIO_3	3

#define KEYMSG_Q_NUM    1  		//������Ϣ���е�����  
#define MESSAGE_Q_NUM   4   	//�������ݵ���Ϣ���е����� 
QueueHandle_t Key_Queue;   		//����ֵ��Ϣ���о��
QueueHandle_t Message_Queue;	//��Ϣ���о��


TaskHandle_t StartTask_Handler;
TaskHandle_t LED0Task_Handler;
TaskHandle_t LED1Task_Handler;
TaskHandle_t task1_task;
TaskHandle_t Keyprocess_task;


extern void start_task(void *pvParameters);
extern void led0_task(void *pvParameters);
extern void led1_task(void *pvParameters);
extern void Keyprocess_Handler(void *pvParameters);
extern void Task1Task_Handler(void *pvParameters);
#endif

