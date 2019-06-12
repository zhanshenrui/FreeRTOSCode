#ifndef __MAIN_H
#define __MAIN_H

#define TSK_SIZE_128	128
#define TSK_SIZE_50		50

#define TSK_PRIO_1	1
#define TSK_PRIO_2	2
#define TSK_PRIO_3	3

#define KEYMSG_Q_NUM    1  		//按键消息队列的数量  
#define MESSAGE_Q_NUM   4   	//发送数据的消息队列的数量 
QueueHandle_t Key_Queue;   		//按键值消息队列句柄
QueueHandle_t Message_Queue;	//信息队列句柄


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

