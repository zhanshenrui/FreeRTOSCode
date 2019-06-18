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
SemaphoreHandle_t BinarySemaphore;


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
	KEY_Init();
    BEEP_Init();
    LCD_Init();
    my_mem_init(SRAMIN);
    POINT_COLOR=RED;

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
    BinarySemaphore=xSemaphoreCreateBinary();
    
	xTaskCreate(task1_task,
		"task1_task",
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		&task1_task_Handler
		);
	xTaskCreate(DataProcess_task,
		"DataProcess_task",
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		&DataProcess_task_Handler
		);
	
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}
//LED0任务函数 
void task1_task(void *pvParameters)
{
    while(1)
    {
        LED0=~LED0;
        vTaskDelay(2000);
    }
}   
u8 CommandProcess(u8* str)
{
    u8 commandval=0;
    if(strcmp((char*)str,"led1on")==0)
        commandval=1;
    else if(strcmp((char*)str,"led1off")==0)
        commandval=2;
    else if(strcmp((char*)str,"led2on")==0)
        commandval=3;
    else if(strcmp((char*)str,"led2off")==0)
        commandval=2;
    return commandval;
}

//LED1任务函数
void DataProcess_task(void *pvParameters)
{
    BaseType_t err;
    u8 len,commandval;
    u8* commandstr;
    while(1)
    {
        if(BinarySemaphore)
        {
            err=xSemaphoreTake(BinarySemaphore, portMAX_DELAY);
            if(err==pdTRUE)
            {
                len=USART_RX_STA&0x3fff;
                commandstr=mymalloc(SRAMIN, len);
                sprintf((char*)commandstr,USART_RX_BUF);
                commandstr[len]='\0';
                commandval=CommandProcess(commandstr);
                switch(commandval)
                {
                    case 1:
                        LED1=ON;
                        break;
                    case 2:
                        LED1=OFF;
                        break;
                    case 3:
                         LED2=ON;
                        break;
                    case 4:
                        LED2=OFF;
                        break;
                }
            USART_RX_STA=0;
            memset(USART_RX_BUF,0,USART_REC_LEN);
            myfree(SRAMIN,commandstr);
                
            }
            else
            {
                vTaskDelay(10);
            }
            
        }
    }
}

void USART1_IRQHandle(void)
{
    u8 Res;
    BaseType_t xHigherPriorityTaskWoken;
    if(USART_GetFlagStatus(USART1, USART_IT_RXNE)!=RESET)
    {
        Res=USART_ReceiveData(USART1);//读取接收到的数据

        if((USART_RX_STA&0x8000)==0)//接收未完成
        {
            if(USART_RX_STA&0x4000)//接收到了0x0d
            {
                if(Res!=0x0a)
                    USART_RX_STA=0;//接收错误
                else
                    USART_RX_STA|=0x8000;//接收完成了
            }
            else//未接收到0x0d
            {
                if(Res==0x0d)//接收到的字符是0x0d
                    USART_RX_STA|=0x4000;
                else//接收到的字符不是0x0d，将新接收到的字符放进数组里
                {
                    USART_RX_BUF[USART_RX_STA&0x3fff]=Res;
                    USART_RX_STA++;
                    if(USART_RX_STA>(USART_REC_LEN-1))
                        USART_RX_STA=0;
                }
            }
        }
    }
    if((USART_RX_STA&0x8000)&&(BinarySemaphore))
    {
        xSemaphoreGiveFromISR(BinarySemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

//外部中断4服务程序
void EXTI3_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line3);  //清除EXTI0线路挂起位
}



