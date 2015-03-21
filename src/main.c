#include "stm32f10x_conf.h"
#include <stdio.h>
#include <math.h>


//#include "rs485.h"
#include "HMWWrapper.h"
#include "eeprom.h"

int Usart1init(void);
int Usart3init(void);
int GPIOinit(void);
int Tim2init(void);
int Systimeinit(void);

void printHardwareValues(void);

uint16_t val2pwm(int val);
void hsv_to_rgb(float* r, float* g, float* b, float h, float s, float v);

//Eeprom thingies:
uint16_t VirtAddVarTab[NumbOfVar] = {0x5555, 0x6666, 0x7777};


volatile int flag=0;
volatile unsigned int systime;	// system Time in msec

/*
 *
 * Virtual EEProm changes:
 * Flash Size is 0x00010000 -> 64KB
 * set it to 0x0000F000 -> 60KB
 * use the last 4 Pages for the virtual EEprom
 * Page 60: at 0x0800F000 - 0x0800F3FF
 * Page 61: at 0x0800F400 - 0x0800F7FF
 * Page 62: at 0x0800F800 - 0x0800FBFF
 * Page 63: at 0x0800FC00 - 0x0800FFFF
 *
 */

int main(void)
{
	SystemInit();

	Usart1init();
	GPIOinit();
	Tim2init();
	Systimeinit();
	//Usart3init();
	//rs485Init(); //now in HMWWrapper

	printHardwareValues();

	// Initialize HM485:
	Init_HMW();


	// testsend:
	//char buffer1[] = "bla\n\r";
	//char buffer2[] = "hallo das ist ein längerer Text\n\r";
	//rs485Write(buffer1, strlen(buffer1));
	//rs485Write(buffer2, 32);

	//GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_SET);		// Ext SW
	GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_RESET);	// Ext SW
	//GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET); // Blue
	//GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET); // RED
	//GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_SET); // Green

	int counter = 0;
	uint16_t bla = 0;
	float r=0,g=0, b=0;

	while(1)
    {
		HMW_loop();

		if(flag==1)
		{
			flag = 0;
			counter++;

			/*
			if(counter == 20)
			{
				int i=0;
				char buffer[50] = {0};
				int ret;
				ret = rs485Read((void*)buffer, 50);
				if(ret != 0)
				{
					//printf("ret=%d buffer: %s \n\r",ret, buffer);
					printf("ret=%d buffer: ",ret);
					for(i = 0; i< ret ; i++)
					{
						printf("%02X ", buffer[i]);
					}
					printf("\n\r",ret);
				}
			}

			if(counter == 400)
			{
			    HMWbroadcast();
				//rs485Write(buffer2, strlen(buffer2));
				//while(1);

			}
            */

			if(counter == 100)
			{

				hsv_to_rgb(&r, &g, &b,(float)bla++ ,1.0 , 0.2);
				TIM_SetCompare1(TIM2, val2pwm((uint16_t)(b*255.0)) );		// blue
				TIM_SetCompare2(TIM2, val2pwm((uint16_t)(r*255.0)) );		// red
				TIM_SetCompare3(TIM2, val2pwm((uint16_t)(g*255.0)) );		// green
				if(bla == 360) bla = 0;
				//printf("%3d\n\r", bla);
				counter = 0;

			}
			/*
			if(counter == 1000)
			{
				GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_SET); // Green
				printf("on\n\r");
			}
			if(counter == 2000)
			{
				GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_RESET); // Green
				counter = 0;
				printf("off\n\r");
			}*/
		}
    }
}

uint16_t val2pwm(int val)
{
	// http://www.mikrocontroller.net/articles/Diskussion:LED-Fading#Diskussion_wissenschaftl.-technischer_Hintergrund
	/*
	float n = 255.0;
	float z = 65536.0;
	float y = 1/2.2;
	uint16_t R = floor((z - 1) * (i / (n - 1)) ^ (1/y) + 0.5) ;
	*/

	uint16_t R = floor(65535.0 * pow( val / 255.0 , 2.2) + 0.5);
	return R;
}

void hsv_to_rgb(float* r, float* g, float* b, float h, float s, float v)	// http://www.mikrocontroller.net/topic/54561
{

	int i;
	float f, p, q, t;

	if( s == 0 )
	{
		*r = *g = *b = v;
		return;
	}

	h /= 60;				// sector 0 to 5
	i = floor( h );
	f = h - i;
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

 switch( i )
 	 	{
		case 0:
			*r = v; *g = t; *b = p; break;

		case 1:
			*r = q; *g = v; *b = p; break;

		case 2:
			*r = p; *g = v; *b = t; break;

		case 3:
			*r = p; *g = q; *b = v; break;

		case 4:
			*r = t; *g = p; *b = v; break;

		default:
			*r = v; *g = p; *b = q; break;				// case 5:
		}

}

void SysTick_Handler(void)
{
	flag=1;
	systime++;
}


int Usart1init(void)
{
	// Debug Serial: USART1
	// PA9  = TX1
	// PA10 = RX1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* TX Pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* RX Pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* USART */
	USART_InitTypeDef USART_InitStructure;

	//USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);

	//USART_ITConfig(USART1, USART_IT_TC, ENABLE);

	/* Enable USART1 */
	USART_Cmd(USART1, ENABLE);

	return 0;
}

int Usart3init(void)
{
	// RS485: USART3
	// PB10 = TX3
	// PB11 = RX3
	// PB9	= DE

	uint8_t usart_transmit_array[] = {'T', 'E', 'S', 'T'};


	/*Clocks*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/*GPIOs*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/* TX Pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* RX Pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* DE Pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* DMA */
	DMA_InitTypeDef DMA_InitStructure;

	/* DMA 1, Channel 2 for USART3 TX */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &usart_transmit_array;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);


	/* USART */
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure);

	//USART_ITConfig(USART1, USART_IT_TC, ENABLE);

	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

	/* Enable USART3 */
	USART_Cmd(USART3, ENABLE);

	printf("curr data counter: %d  \n", DMA_GetCurrDataCounter(DMA1_Channel2));

	DMA_Cmd(DMA1_Channel2, DISABLE);
	GPIO_WriteBit(GPIOB, GPIO_Pin_9, SET);
	DMA_SetCurrDataCounter(DMA1_Channel2, 3);
	DMA_ClearFlag(DMA1_FLAG_TC2);

	printf("curr data counter: %d  \n", DMA_GetCurrDataCounter(DMA1_Channel2));
	printf("dmaflag GL: %d \n",DMA_GetFlagStatus(DMA1_FLAG_GL2) );
	printf("dmaflag TC: %d \n",DMA_GetFlagStatus(DMA1_FLAG_TC2) );
	printf("dmaflag HT: %d \n",DMA_GetFlagStatus(DMA1_FLAG_HT2) );
	printf("dmaflag TE: %d \n",DMA_GetFlagStatus(DMA1_FLAG_TE2) );

	DMA_Cmd(DMA1_Channel2, ENABLE);

	printf("curr data counter: %d  \n", DMA_GetCurrDataCounter(DMA1_Channel2));
	printf("dmaflag GL: %d \n",DMA_GetFlagStatus(DMA1_FLAG_GL2) );
	printf("dmaflag TC: %d \n",DMA_GetFlagStatus(DMA1_FLAG_TC2) );
	printf("dmaflag HT: %d \n",DMA_GetFlagStatus(DMA1_FLAG_HT2) );
	printf("dmaflag TE: %d \n",DMA_GetFlagStatus(DMA1_FLAG_TE2) );

	//Usart3Put('B');
	//Usart3Put('L');
	//Usart3Put('A');
	while (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET)
	  {
		printf("dma not complete\n " );
	  }
	return 0;
}

int GPIOinit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* PA0 Pin  T2_CH1*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA1 Pin  T2_CH2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA2 Pin  T2_CH3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA3 Pin  T2_CH4*/ 	// Ext Sw
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	return 0;
}

int Systimeinit(void)
{
	SysTick_Config((SystemCoreClock/1000) -1);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_SetPriority(SysTick_IRQn, 14);
	return 0;
}

int Tim2init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	TIM_OCInitTypeDef TIM_OC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* PA0 Pin  T2_CH1*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA1 Pin  T2_CH2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA2 Pin  T2_CH3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_Period = 0xFFFF; // 9999;
	TIM_TimeBase_InitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBase_InitStructure);

	TIM_OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OC_InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC_InitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	TIM_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC_InitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_InitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OC_InitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM2, &TIM_OC_InitStructure);
	TIM_OC2Init(TIM2, &TIM_OC_InitStructure);
	TIM_OC3Init(TIM2, &TIM_OC_InitStructure);
	TIM_Cmd(TIM2, ENABLE);

	return 0;
}

void Usart1Put(uint8_t ch)
{
      USART_SendData(USART1, (uint8_t) ch);
      //Loop until the end of transmission
      while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      {
      }
}
uint8_t Usart1Get(void){
     while ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
        return (uint8_t)USART_ReceiveData(USART1);
}

void Usart3Put(uint8_t ch)
{
      USART_SendData(USART3, (uint8_t) ch);
      //Loop until the end of transmission
      while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
      {
      }
}
uint8_t Usart3Get(void){
     while ( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
        return (uint8_t)USART_ReceiveData(USART3);
}

void printHardwareValues(void)
{
	// Unique ID:
	uint32_t *uid;
	uid = (uint32_t*)0x1FFFF7E8;
	printf("UID[0]: 0x%08X\n\r", uid[0]);
	printf("UID[1]: 0x%08X\n\r", uid[1]);
	printf("UID[2]: 0x%08X\n\r", uid[2]);
	printf("UID[3]: 0x%08X\n\r", uid[3]);

	// Flash Size:
	uint16_t *s;
	s = (uint16_t*)0x1FFFF7E0;
	printf("Flash Size: 0x%04X = %dKB \n\r", *s, *s);

	// MCU ID code
	uint16_t *mcuid;
	mcuid = (uint16_t*)0xE0042000;

	printf("DEV_ID: 0x%04X\n\r", mcuid[0] & 0xfff);
	printf("REV_ID: 0x%04X\n\r", mcuid[1]);
}





