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

tim3中断函数必须写，因为FreeRTOS用了TIM3作为任务统计定时器并开了中断，如果不写就会因为发生中断但找不到中断入口函数而卡在startup文件处的B . ENDP
这个任务是获取输入的，不能延时太长，延时太长会获取不到输入，这里延时10ms,一般的做法是将输入用中断，然后在中断里发送信号量

*/
#include "main.h"
int lcd_discolor[9]={BLUE,BRED,GRED,GBLUE,RED,MAGENTA,GREEN,YELLOW,BROWN};
SemaphoreHandle_t   CountSemaphore;


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
		&StartTask_Handler
		);
	vTaskStartScheduler();
}

void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();
    CountSemaphore=xSemaphoreCreateCounting(255,0);
	xTaskCreate(SemapGive_task,
		"SemapGive_task",
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		&SemapGive_task_Handler
		);
	xTaskCreate(SemapTake_task,
		"SemapTake_task",
		TSK_SIZE_128,
		NULL,
		TSK_PRIO_1,
		&SemapTake_task_Handler
		);
	
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}
//LED0任务函数 
//这个任务是获取输入的，不能延时太长，延时太长会获取不到输入，这里延时10ms,一般的做法是将输入用中断，然后在中断里发送信号量
void SemapGive_task(void *pvParameters)
{
    u8 key,semavalue,i;
    BaseType_t err;
    while(1)
    {
        if(CountSemaphore)
        {
            key=KEY_Scan(0);
            if(key==KEY0_PRES)
            {
                err=xSemaphoreGive(CountSemaphore);
                semavalue=uxSemaphoreGetCount(CountSemaphore);
                LCD_ShowNum(155,111,semavalue,3,16);
            }
            i++;
            if(i==50)
            {
                i=0;
                LED2=~LED2;
            }
            vTaskDelay(10);
        }
    }
}   

//LED1任务函数
void SemapTake_task(void *pvParameters)
{
    u8 semavalue,num;
    while(1)
    {
        xSemaphoreTake(CountSemaphore, portMAX_DELAY);
        semavalue=uxSemaphoreGetCount(CountSemaphore);
        LCD_ShowNum(155,111,semavalue,3,16);
        LCD_Fill(6,131,233,313,lcd_discolor[num%9]);
        vTaskDelay(1000);
    }
}
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	BaseType_t xHigherPriorityTaskWoken;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{//printf("a");
		Res =USART_ReceiveData(USART1);	//读取接收到的数据

		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{	
				if(Res!=0x0a)
					USART_RX_STA=0;//接收错误,重新开始
				else 
					USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)
					USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}
		}

   

} 
//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		FreeRTOSRunTimeTicks++;//这里到底执不执行？因为TIM3的中断优先级比OS的能管理的优先级还高
		//printf("run tim3\r\n");//如果用打印，程序会很卡
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
//外部中断4服务程序
void EXTI3_IRQHandler(void)
{//printf("b");
	EXTI_ClearITPendingBit(EXTI_Line3);  //清除EXTI0线路挂起位
}



