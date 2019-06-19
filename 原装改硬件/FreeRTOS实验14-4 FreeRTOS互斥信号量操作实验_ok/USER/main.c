#include "main.h"

/************************************************
 ALIENTEK 战舰STM32F103开发板 FreeRTOS实验14-4
 FreeRTOS互斥信号量操作实验-库函数版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/



//互斥信号量句柄
SemaphoreHandle_t MutexSemaphore;	//互斥信号量

//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	 
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
	KEY_Init();							//初始化按键
	BEEP_Init();						//初始化蜂鸣器
	LCD_Init();							//初始化LCD
	my_mem_init(SRAMIN);            	//初始化内部内存池

    POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"ATK STM32F103/407");	
	LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 14-3");
	LCD_ShowString(30,50,200,16,16,"Priority Overturn");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2016/11/25");
    printf("test\r\n");
    //while(1);
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )TSK_SIZE_128,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )TSK_PRIO_1,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
	//创建互斥信号量
	MutexSemaphore=xSemaphoreCreateMutex();
    //xSemaphoreGive(MutexSemaphore);
	
    //创建高优先级任务
    xTaskCreate((TaskFunction_t )high_task,             
                (const char*    )"high_task",           
                (uint16_t       )TSK_SIZE_128,        
                (void*          )NULL,                  
                (UBaseType_t    )TSK_PRIO_3,        
                (TaskHandle_t*  )&HighTask_Handler);   
    //创建中等优先级任务
    xTaskCreate((TaskFunction_t )middle_task,     
                (const char*    )"middle_task",   
                (uint16_t       )TSK_SIZE_128,
                (void*          )NULL,
                (UBaseType_t    )TSK_PRIO_2,
                (TaskHandle_t*  )&MiddleTask_Handler); 
	//创建低优先级任务
    xTaskCreate((TaskFunction_t )low_task,     
                (const char*    )"low_task",   
                (uint16_t       )TSK_SIZE_128,
                (void*          )NULL,
                (UBaseType_t    )TSK_PRIO_1,
                (TaskHandle_t*  )&LowTask_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//高优先级任务的任务函数
void high_task(void *pvParameters)
{
	u8 num;
	
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(5,110,115,314); 	//画一个矩形	
	LCD_DrawLine(5,130,115,130);		//画线
	POINT_COLOR = BLUE;
	LCD_ShowString(6,111,110,16,16,"High Task");
	
	while(1)
	{
		vTaskDelay(500);	//延时500ms，也就是500个时钟节拍	
		num++;
		printf("high task Pend Sem\r\n");
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//获取互斥信号量
		printf("high task Running!\r\n");
		LCD_Fill(6,131,114,313,lcd_discolor[num%14]); 	//填充区域
		LED2=!LED2;
		xSemaphoreGive(MutexSemaphore);					//释放信号量
		vTaskDelay(500);	//延时500ms，也就是500个时钟节拍  
	}
}

//中等优先级任务的任务函数
void middle_task(void *pvParameters)
{
	u8 num;
	
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(125,110,234,314); //画一个矩形	
	LCD_DrawLine(125,130,234,130);		//画线
	POINT_COLOR = BLUE;
	LCD_ShowString(126,111,110,16,16,"Middle Task");
	while(1)
	{
		num++;
		printf("middle task Running!\r\n");
		LCD_Fill(126,131,233,313,lcd_discolor[13-num%14]); //填充区域
		LED1=!LED1;
        vTaskDelay(1000);	//延时1s，也就是1000个时钟节拍	
	}
}

//低优先级任务的任务函数
void low_task(void *pvParameters)
{
	static u32 times;

	while(1)
	{
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//获取互斥信号量
		printf("low task Running!\r\n");
		for(times=0;times<5000000;times++)				//模拟低优先级任务占用互斥信号量
		{
			taskYIELD();								//发起任务调度
		}
		xSemaphoreGive(MutexSemaphore);					//释放互斥信号量
		LED0=!LED0;		
		vTaskDelay(1000);	//延时1s，也就是1000个时钟节拍	
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



