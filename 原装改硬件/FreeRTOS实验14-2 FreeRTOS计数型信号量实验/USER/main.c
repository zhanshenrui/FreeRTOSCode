/*
xTaskCreate创建任务
vTaskDelete删除任务
vTaskStartScheduler启动系统，开始任务调度
vTaskDelay系统延时，可以让任务切换
delay_ms可以让任务切换的延时，实际是调用vTaskDelay来让任务切换
delay_us，delay_xms不能让任务切换

xTaskCreate第一个参数是任务函数，第二个参数是任务名字，以便调试用，第三个参数是任务堆栈大小，
第四个参数是任务函数的参数列表，第五个参数是任务的优先级，第六个参数是任务句柄

注意接收队列时要设置阻塞时间，发送队列时注意要判断是否进行任务调度即调用函数portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
其中xHigherPriorityTaskWoken由用户自己定义，但在接收队列时会被赋值，然后再根据这个值由OS来判断是否发生调度portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
定时器3定时器5没有初始化，所以不能用

LCD输出比串口输出的优点：在固定位置输出，不会输出很多内容

STM32F103ZET6的SRAM为64K，但FreeRTOS也占用了内存，而且占用比较大，这样，如果内存管理还管理40K，就会出现hardfault错误，但改成20K就没问题，即
#define MEM1_MAX_SIZE			40*1024  	会有问题，要改成
#define MEM1_MAX_SIZE			20*1024  	这样就没问题
*/
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
//LED0任务函数 
void led0_task(void *pvParameters)
{
    while(1)
    {
        LED2=~LED2;
        vTaskDelay(2000);
    }
}   

//LED1任务函数
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


