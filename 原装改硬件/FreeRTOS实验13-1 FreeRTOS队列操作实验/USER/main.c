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

#define KEYMSG_Q_NUM    1  		//按键消息队列的数量  
#define MESSAGE_Q_NUM   4   	//发送数据的消息队列的数量 
QueueHandle_t Key_Queue;   		//按键值消息队列句柄
QueueHandle_t Message_Queue;	//信息队列句柄
//volatile unsigned long long FreeRTOSRunTimeTicks;
//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };


//查询Message_Queue队列中的总队列数量和剩余队列数量
//用于在LCD上显示接收到的队列的消息
//str: 要显示的字符串(接收到的消息)
void disp_str(u8* str)
{
	LCD_Fill(5,230,110,245,WHITE);					//先清除显示区域
	LCD_ShowString(5,230,100,16,16,str);
}

void freertos_load_main_ui(void)
{
	POINT_COLOR = RED;
	LCD_ShowString(10,10,200,16,16,"ATK STM32F103/407");	
	LCD_ShowString(10,30,200,16,16,"FreeRTOS Examp 13-1");
	LCD_ShowString(10,50,200,16,16,"Message Queue");
	LCD_ShowString(10,70,220,16,16,"KEY_UP:LED1 KEY0:Refresh LCD");
	LCD_ShowString(10,90,200,16,16,"KEY1:SendMsg KEY2:BEEP");
	
	POINT_COLOR = BLACK;
	LCD_DrawLine(0,107,239,107);		//画线
	LCD_DrawLine(119,107,119,319);		//画线
	LCD_DrawRectangle(125,110,234,314);	//画矩形
	POINT_COLOR = RED;
	LCD_ShowString(0,130,120,16,16,"DATA_Msg Size:");
	LCD_ShowString(0,170,120,16,16,"DATA_Msg rema:");
	LCD_ShowString(0,210,100,16,16,"DATA_Msg:");
	POINT_COLOR = BLUE;
}

void check_msg_queue(void)
{
   u8 *p;
	u8 msgq_remain_size;
	u8 msgq_total_size;
	taskENTER_CRITICAL();
	msgq_remain_size=uxQueueSpacesAvailable(Message_Queue);
	msgq_total_size=uxQueueMessagesWaiting( Message_Queue )+uxQueueSpacesAvailable(Message_Queue);
	p=mymalloc(SRAMIN,20);	//申请内存
	sprintf((char*)p,"Total Size:%d",msgq_total_size);	//显示DATA_Msg消息队列总的大小
	LCD_ShowString(10,150,100,16,16,p);
	sprintf((char*)p,"Remain Size:%d",msgq_remain_size);	//显示DATA_Msg剩余大小
	LCD_ShowString(10,190,100,16,16,p);
	myfree(SRAMIN,p);		//释放内存
	taskEXIT_CRITICAL();
}
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
    KEY_Init();
    BEEP_Init();
	LCD_Init();
	TIM2_Int_Init(5000,7200-1);
	my_mem_init(SRAMIN);            	//初始化内部内存池
    freertos_load_main_ui();        	//加载主UI
   // while(1)
    	{
    	printf("hello\r\n");
				//LCD_ShowString(00,16*4,8*16,16,16,"WAKEUP_KEY PRESS");
    	}
	
	//LCD_ShowString(00,16*4,8*16,16,16,"WAKEUP_KEY PRESS");


	xTaskCreate(start_task,
		"start_task",
		TSK_SIZE_256,
		NULL,
		TSK_PRIO_1,
		&StartTask_Handler
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
		TSK_SIZE_256,
		NULL,
		TSK_PRIO_1,
		&Task1Task_Handler
		);
	xTaskCreate(Keyprocess_task,
		"Keyprocess_task",
		TSK_SIZE_256,
		NULL,
		TSK_PRIO_1,
		&Keyprocess_Handler
		);
	
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}
//LED0任务函数 
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

//LED1任务函数
void Keyprocess_task(void *pvParameters)
{
    u8 num,getkey;
    while(1)
    {
        if(Key_Queue)
        {
            if(xQueueReceive(Key_Queue, &getkey, portMAX_DELAY))
            {
                switch(getkey)
                {
                    case KEY0_PRES:
                        LED1=!LED1;
                        break;
                    case KEY1_PRES:
                        BEEP=!BEEP;
                        break;
                    case KEY2_PRES:
                        num++;
                        LCD_Fill(126,111,233,313,lcd_discolor[num%14]);
                       printf("num++\r\n");
												break;
                }
            }
        }
        vTaskDelay(10);
    }
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	BaseType_t xHigherPriorityTaskWoken;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
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

   
	if((USART_RX_STA&0x8000)&&(Message_Queue!=NULL))
		{
			xQueueSendFromISR(Message_Queue, USART_RX_BUF, &xHigherPriorityTaskWoken);
			USART_RX_STA=0;	
			memset(USART_RX_BUF,0,USART_REC_LEN);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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

void TIM2_IRQHandler(void)
{
	u8 *buffer;
	//u8 buffer[200];
	BaseType_t xTaskWokenByReceive=pdFALSE;
	BaseType_t err;
	
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
		buffer=mymalloc(SRAMIN,USART_REC_LEN);
        if(Message_Queue!=NULL)
        {
			memset(buffer,0,USART_REC_LEN);	//清除缓冲区
			err=xQueueReceiveFromISR(Message_Queue,buffer,&xTaskWokenByReceive);//请求消息Message_Queue
            if(err==pdTRUE)			//接收到消息
            {
				disp_str(buffer);	//在LCD上显示接收到的消息
				printf("buff=%s\r\n",buffer);
            }
        }
		myfree(SRAMIN,buffer);
		portYIELD_FROM_ISR(xTaskWokenByReceive);//如果需要的话进行一次任务切换
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}

//定时器5中断服务函数
void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //溢出中断
	{
		printf("TIM5输出.......\r\n");
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //清除中断标志位
}




