#ifndef __MAIN_H
#define __MAIN_H

#define TSK_SIZE_128	128
#define TSK_SIZE_50		50

#define TSK_PRIO_1	1
#define TSK_PRIO_2	2
#define TSK_PRIO_3	3

TaskHandle_t StartTask_Handler;
TaskHandle_t LED0Task_Handler;
TaskHandle_t LED1Task_Handler;

extern void start_task(void *pvParameters);
extern void led0_task(void *pvParameters);
extern void led1_task(void *pvParameters);
#endif

