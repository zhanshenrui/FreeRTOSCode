/*
xTaskCreate��������
vTaskDeleteɾ������
vTaskStartScheduler����ϵͳ����ʼ�������
vTaskDelayϵͳ��ʱ�������������л�
delay_ms�����������л�����ʱ��ʵ���ǵ���vTaskDelay���������л�
delay_us��delay_xms�����������л�

xTaskCreate��һ�����������������ڶ����������������֣��Ա�����ã������������������ջ��С��
���ĸ��������������Ĳ����б��������������������ȼ���������������������

ע����ն���ʱҪ��������ʱ�䣬���Ͷ���ʱע��Ҫ�ж��Ƿ����������ȼ����ú���portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
����xHigherPriorityTaskWoken���û��Լ����壬���ڽ��ն���ʱ�ᱻ��ֵ��Ȼ���ٸ������ֵ��OS���ж��Ƿ�������portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
��ʱ��3��ʱ��5û�г�ʼ�������Բ�����

LCD����ȴ���������ŵ㣺�ڹ̶�λ���������������ܶ�����

STM32F103ZET6��SRAMΪ64K����FreeRTOSҲռ�����ڴ棬����ռ�ñȽϴ�����������ڴ��������40K���ͻ����hardfault���󣬵��ĳ�20K��û���⣬��
#define MEM1_MAX_SIZE			40*1024  	�������⣬Ҫ�ĳ�
#define MEM1_MAX_SIZE			20*1024  	������û����
*/
#include "main.h"

#define KEYMSG_Q_NUM    1  		//������Ϣ���е�����  
#define MESSAGE_Q_NUM   4   	//�������ݵ���Ϣ���е����� 
QueueHandle_t Key_Queue;   		//����ֵ��Ϣ���о��
QueueHandle_t Message_Queue;	//��Ϣ���о��
//volatile unsigned long long FreeRTOSRunTimeTicks;
//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };


//��ѯMessage_Queue�����е��ܶ���������ʣ���������
//������LCD����ʾ���յ��Ķ��е���Ϣ
//str: Ҫ��ʾ���ַ���(���յ�����Ϣ)
void disp_str(u8* str)
{
	LCD_Fill(5,230,110,245,WHITE);					//�������ʾ����
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
	LCD_DrawLine(0,107,239,107);		//����
	LCD_DrawLine(119,107,119,319);		//����
	LCD_DrawRectangle(125,110,234,314);	//������
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
	p=mymalloc(SRAMIN,20);	//�����ڴ�
	sprintf((char*)p,"Total Size:%d",msgq_total_size);	//��ʾDATA_Msg��Ϣ�����ܵĴ�С
	LCD_ShowString(10,150,100,16,16,p);
	sprintf((char*)p,"Remain Size:%d",msgq_remain_size);	//��ʾDATA_Msgʣ���С
	LCD_ShowString(10,190,100,16,16,p);
	myfree(SRAMIN,p);		//�ͷ��ڴ�
	taskEXIT_CRITICAL();
}
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(115200);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
    KEY_Init();
    BEEP_Init();
	LCD_Init();
	TIM2_Int_Init(5000,7200-1);
	my_mem_init(SRAMIN);            	//��ʼ���ڲ��ڴ��
    freertos_load_main_ui();        	//������UI
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

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	BaseType_t xHigherPriorityTaskWoken;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
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

   
	if((USART_RX_STA&0x8000)&&(Message_Queue!=NULL))
		{
			xQueueSendFromISR(Message_Queue, USART_RX_BUF, &xHigherPriorityTaskWoken);
			USART_RX_STA=0;	
			memset(USART_RX_BUF,0,USART_REC_LEN);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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

void TIM2_IRQHandler(void)
{
	u8 *buffer;
	//u8 buffer[200];
	BaseType_t xTaskWokenByReceive=pdFALSE;
	BaseType_t err;
	
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //����ж�
	{
		buffer=mymalloc(SRAMIN,USART_REC_LEN);
        if(Message_Queue!=NULL)
        {
			memset(buffer,0,USART_REC_LEN);	//���������
			err=xQueueReceiveFromISR(Message_Queue,buffer,&xTaskWokenByReceive);//������ϢMessage_Queue
            if(err==pdTRUE)			//���յ���Ϣ
            {
				disp_str(buffer);	//��LCD����ʾ���յ�����Ϣ
				printf("buff=%s\r\n",buffer);
            }
        }
		myfree(SRAMIN,buffer);
		portYIELD_FROM_ISR(xTaskWokenByReceive);//�����Ҫ�Ļ�����һ�������л�
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
}

//��ʱ��5�жϷ�����
void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //����ж�
	{
		printf("TIM5���.......\r\n");
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //����жϱ�־λ
}




