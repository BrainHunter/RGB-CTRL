#include "stm32f10x_conf.h"
#include <stdio.h>
#include <math.h>


//#include "rs485.h"
#include "HMWWrapper.h"
#include "eeprom.h"
#include "rgb.h"

int Usart1init(void);
int Usart3init(void);
int GPIOinit(void);
int Systimeinit(void);

void printHardwareValues(void);

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
	RGB_init();
	Systimeinit();

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

	while(1)
    {
		HMW_loop();

        RGB_loop();

		if(flag==1)
		{
			flag = 0;
            // do sth. in 1 msec interval
		}
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



int Systimeinit(void)
{
	SysTick_Config((SystemCoreClock/1000) -1);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_SetPriority(SysTick_IRQn, 14);
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


void printHardwareValues(void)
{
	// Unique ID:
	uint32_t *uid;
	uid = (uint32_t*)0x1FFFF7E8;
	printf("UID[0]: 0x%08X\n\r", (unsigned int)uid[0]);
	printf("UID[1]: 0x%08X\n\r", (unsigned int)uid[1]);
	printf("UID[2]: 0x%08X\n\r", (unsigned int)uid[2]);
	printf("UID[3]: 0x%08X\n\r", (unsigned int)uid[3]);

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





