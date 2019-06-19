#include "main.h"

/************************************************
 ALIENTEK ս��STM32F103������ FreeRTOSʵ��14-4
 FreeRTOS�����ź�������ʵ��-�⺯���汾
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/



//�����ź������
SemaphoreHandle_t MutexSemaphore;	//�����ź���

//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				//��ʱ������ʼ��	 
	uart_init(115200);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
	KEY_Init();							//��ʼ������
	BEEP_Init();						//��ʼ��������
	LCD_Init();							//��ʼ��LCD
	my_mem_init(SRAMIN);            	//��ʼ���ڲ��ڴ��

    POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"ATK STM32F103/407");	
	LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 14-3");
	LCD_ShowString(30,50,200,16,16,"Priority Overturn");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2016/11/25");
    printf("test\r\n");
    //while(1);
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )TSK_SIZE_128,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )TSK_PRIO_1,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
	
	//���������ź���
	MutexSemaphore=xSemaphoreCreateMutex();
    //xSemaphoreGive(MutexSemaphore);
	
    //���������ȼ�����
    xTaskCreate((TaskFunction_t )high_task,             
                (const char*    )"high_task",           
                (uint16_t       )TSK_SIZE_128,        
                (void*          )NULL,                  
                (UBaseType_t    )TSK_PRIO_3,        
                (TaskHandle_t*  )&HighTask_Handler);   
    //�����е����ȼ�����
    xTaskCreate((TaskFunction_t )middle_task,     
                (const char*    )"middle_task",   
                (uint16_t       )TSK_SIZE_128,
                (void*          )NULL,
                (UBaseType_t    )TSK_PRIO_2,
                (TaskHandle_t*  )&MiddleTask_Handler); 
	//���������ȼ�����
    xTaskCreate((TaskFunction_t )low_task,     
                (const char*    )"low_task",   
                (uint16_t       )TSK_SIZE_128,
                (void*          )NULL,
                (UBaseType_t    )TSK_PRIO_1,
                (TaskHandle_t*  )&LowTask_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//�����ȼ������������
void high_task(void *pvParameters)
{
	u8 num;
	
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(5,110,115,314); 	//��һ������	
	LCD_DrawLine(5,130,115,130);		//����
	POINT_COLOR = BLUE;
	LCD_ShowString(6,111,110,16,16,"High Task");
	
	while(1)
	{
		vTaskDelay(500);	//��ʱ500ms��Ҳ����500��ʱ�ӽ���	
		num++;
		printf("high task Pend Sem\r\n");
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//��ȡ�����ź���
		printf("high task Running!\r\n");
		LCD_Fill(6,131,114,313,lcd_discolor[num%14]); 	//�������
		LED2=!LED2;
		xSemaphoreGive(MutexSemaphore);					//�ͷ��ź���
		vTaskDelay(500);	//��ʱ500ms��Ҳ����500��ʱ�ӽ���  
	}
}

//�е����ȼ������������
void middle_task(void *pvParameters)
{
	u8 num;
	
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(125,110,234,314); //��һ������	
	LCD_DrawLine(125,130,234,130);		//����
	POINT_COLOR = BLUE;
	LCD_ShowString(126,111,110,16,16,"Middle Task");
	while(1)
	{
		num++;
		printf("middle task Running!\r\n");
		LCD_Fill(126,131,233,313,lcd_discolor[13-num%14]); //�������
		LED1=!LED1;
        vTaskDelay(1000);	//��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}

//�����ȼ������������
void low_task(void *pvParameters)
{
	static u32 times;

	while(1)
	{
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//��ȡ�����ź���
		printf("low task Running!\r\n");
		for(times=0;times<5000000;times++)				//ģ������ȼ�����ռ�û����ź���
		{
			taskYIELD();								//�����������
		}
		xSemaphoreGive(MutexSemaphore);					//�ͷŻ����ź���
		LED0=!LED0;		
		vTaskDelay(1000);	//��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	BaseType_t xHigherPriorityTaskWoken;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{//printf("a");
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������

		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{	
				if(Res!=0x0a)
					USART_RX_STA=0;//���մ���,���¿�ʼ
				else 
					USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)
					USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}
		}

   

} 
//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		FreeRTOSRunTimeTicks++;//���ﵽ��ִ��ִ�У���ΪTIM3���ж����ȼ���OS���ܹ�������ȼ�����
		//printf("run tim3\r\n");//����ô�ӡ�������ܿ�
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}
//�ⲿ�ж�4�������
void EXTI3_IRQHandler(void)
{//printf("b");
	EXTI_ClearITPendingBit(EXTI_Line3);  //���EXTI0��·����λ
}



