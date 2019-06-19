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

tim3�жϺ�������д����ΪFreeRTOS����TIM3��Ϊ����ͳ�ƶ�ʱ���������жϣ������д�ͻ���Ϊ�����жϵ��Ҳ����ж���ں���������startup�ļ�����B . ENDP
��������ǻ�ȡ����ģ�������ʱ̫������ʱ̫�����ȡ�������룬������ʱ10ms,һ��������ǽ��������жϣ�Ȼ�����ж��﷢���ź���

*/
#include "main.h"
int lcd_discolor[9]={BLUE,BRED,GRED,GBLUE,RED,MAGENTA,GREEN,YELLOW,BROWN};
SemaphoreHandle_t   CountSemaphore;


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
//LED0������ 
//��������ǻ�ȡ����ģ�������ʱ̫������ʱ̫�����ȡ�������룬������ʱ10ms,һ��������ǽ��������жϣ�Ȼ�����ж��﷢���ź���
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

//LED1������
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



