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
SemaphoreHandle_t BinarySemaphore;


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(115200);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
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
//LED0������ 
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

//LED1������
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
        Res=USART_ReceiveData(USART1);//��ȡ���յ�������

        if((USART_RX_STA&0x8000)==0)//����δ���
        {
            if(USART_RX_STA&0x4000)//���յ���0x0d
            {
                if(Res!=0x0a)
                    USART_RX_STA=0;//���մ���
                else
                    USART_RX_STA|=0x8000;//���������
            }
            else//δ���յ�0x0d
            {
                if(Res==0x0d)//���յ����ַ���0x0d
                    USART_RX_STA|=0x4000;
                else//���յ����ַ�����0x0d�����½��յ����ַ��Ž�������
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

//�ⲿ�ж�4�������
void EXTI3_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line3);  //���EXTI0��·����λ
}



