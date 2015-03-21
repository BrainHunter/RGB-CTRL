#include "rs485.h"
#include "stm32f10x_conf.h"
//#include <stdint.h>



volatile uint8_t sendBuffer[SEND_BUFFER_SIZE];
volatile int sendBuffer_WP = 0;
volatile int sendBuffer_RP = 0;
volatile int sendBuffer_DMA = 0;


volatile uint8_t receiveBuffer[RECEIVE_BUFFER_SIZE*2];

volatile int receiveBuffer_RP = 0;

int SendSpace (void);

DMA_InitTypeDef DMA_TXStructure;
DMA_InitTypeDef DMA_RXStructure;

void rs485Init(void)
{
	// RS485: USART3
		// PB10 = TX3
		// PB11 = RX3
		// PB9	= DE


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

		/*Interrupts*/
		NVIC_InitTypeDef NVIC_InitStructure;

		/* DMA Channel 2 (USART TC IR) */
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		/* USART3 (RX)*/
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);


		/* DMA */
		//DMA_InitTypeDef DMA_TXStructure;

		/* DMA 1, Channel 2 for USART3 TX */
		DMA_TXStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
		DMA_TXStructure.DMA_MemoryBaseAddr = (uint32_t) &sendBuffer;
		DMA_TXStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_TXStructure.DMA_BufferSize = 0;
		DMA_TXStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_TXStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_TXStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_TXStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_TXStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_TXStructure.DMA_Priority = DMA_Priority_VeryHigh;
		DMA_TXStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel2, &DMA_TXStructure);

		/* DMA 1, Channel 3 for USART3 RX */
		DMA_RXStructure = DMA_TXStructure;
		DMA_DeInit(DMA1_Channel3);
		DMA_RXStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
		DMA_RXStructure.DMA_MemoryBaseAddr = (uint32_t) &receiveBuffer;
		DMA_RXStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_RXStructure.DMA_BufferSize = RECEIVE_BUFFER_SIZE;
		DMA_RXStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_Init(DMA1_Channel3, &DMA_RXStructure);

		/* Enable DMA1_Channel Transfer Complete interrupt */
		DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

		/* USART */
		USART_InitTypeDef USART_InitStructure;

		USART_InitStructure.USART_BaudRate = 19200;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART3, &USART_InitStructure);

		//activate Transmission Complete interrupt
		USART_ITConfig(USART3, USART_IT_TC, ENABLE);
		/* Enable USART_Rx Receive interrupt */
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

		// Enable DMA Requests
		USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
		USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

		/* Enable USART3 */
		USART_Cmd(USART3, ENABLE);

		// enable RX DMA
		DMA_Cmd(DMA1_Channel3, ENABLE);

/*

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
*/
		return ;

}

int rs485Write(void *buffer, int len)
{
	// check free space in buffer:
	int freespace = SendSpace();

	if(len > freespace)
	{
		len = freespace;	// only send the amount we can
	}
	// copy data to Write Buffer:
	int temp = len;
	int dmaSize = 0;
	int dmaStart = sendBuffer_WP;
	while(temp--)
	{
		sendBuffer[sendBuffer_WP++] = *((uint8_t*)buffer++);
		// wrap on buffer end:
		if(sendBuffer_WP == SEND_BUFFER_SIZE)
		{
			sendBuffer_WP = 0;
			dmaSize = len - temp;
		}
	}

	// is the last DMA transfer complete?
	if(sendBuffer_RP == dmaStart)
	{
		// Enable DE
		GPIO_WriteBit(GPIOB, GPIO_Pin_9, SET);

		// trigger dma transfer
		if(dmaSize == 0) dmaSize = len;

		// mark buffer position for dma transfer
		sendBuffer_DMA = (dmaStart+dmaSize);
		if(sendBuffer_DMA == SEND_BUFFER_SIZE)
		{

			sendBuffer_DMA = 0;	// for wrap around
		}

		DMA_Cmd(DMA1_Channel2, DISABLE);
		//DMA_DeInit(DMA1_Channel2);
		DMA_TXStructure.DMA_MemoryBaseAddr = (uint32_t) ((uint8_t*)(&sendBuffer) + dmaStart);
		DMA_TXStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_TXStructure.DMA_BufferSize = dmaSize;
		DMA_Init(DMA1_Channel2, &DMA_TXStructure);
		DMA_ClearFlag(DMA1_FLAG_TC2);
		//DMA_SetCurrDataCounter(DMA1_Channel2, dmaSize);
		DMA_Cmd(DMA1_Channel2, ENABLE);

	}
	return len;
}

int rs485Read(void *buffer, int len)
{
	int counter = 0;
	while(receiveBuffer_RP != (RECEIVE_BUFFER_SIZE -  DMA_GetCurrDataCounter(DMA1_Channel3)))
	{
		// copy the next byte:
		*((uint8_t*)buffer++) = receiveBuffer[receiveBuffer_RP++];
		counter++;
		// wrap around:
		if(receiveBuffer_RP == RECEIVE_BUFFER_SIZE)
		{
			receiveBuffer_RP = 0;
		}

		// output buffer is full
		if(counter == len) break;
	}
	//int dma_counter = DMA_GetCurrDataCounter(DMA1_Channel3);
	//int temp = RECEIVE_BUFFER_SIZE - dma_counter;
	return counter;
}


int SendSpace (void)
{
	int size = 0; //SEND_BUFFER_SIZE;
	if(sendBuffer_WP >= sendBuffer_RP)
	{
		size = SEND_BUFFER_SIZE - (sendBuffer_WP - sendBuffer_RP) - 1;
	}
	else
	{
		size = sendBuffer_RP - sendBuffer_WP - 1;
	}
	return size;
}

int rs485BytesAvailable(void)
{
	int size = 0;
	int DMA_DataCounter = (RECEIVE_BUFFER_SIZE -  DMA_GetCurrDataCounter(DMA1_Channel3));
	if(receiveBuffer_RP > DMA_DataCounter)
	{
		size = RECEIVE_BUFFER_SIZE-(receiveBuffer_RP -DMA_DataCounter )-1;
	}
	else
	{
		size = DMA_DataCounter - receiveBuffer_RP ;
	}
	return size;
}

void USART3_IRQHandler(void)
{
	// Transmission Complete
	if( USART_GetITStatus(USART3, USART_IT_TC) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_TC);
		// end Transfer
		GPIO_WriteBit(GPIOB, GPIO_Pin_9, RESET);                // DE Pin low
	}

	// Overrun Error Detected
	if( USART_GetITStatus(USART3, USART_IT_ORE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_ORE);
	}

	// Received Data ready to be read
	if( USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
#if 0
		do
		{
			/* Fill the buffer RxBuffer2_SW used for final data store  */
			RxBuffer2_SW[RxCounter1++] = RxBuffer2[RxCounter++];

		}  while((RxCounter < (FIFO_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6))));


		if(RxCounter == FIFO_SIZE)
		{
			RxCounter = 0;
		}
#endif
	}
}

// USART TX DMA!
void DMA1_Channel2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_FLAG_TC2))
	{
		/* all data transmitted */
		DMA_ClearFlag(DMA1_FLAG_TC2);

  	  	sendBuffer_RP = sendBuffer_DMA;

  		// is there more data to send?
  		if(sendBuffer_RP != sendBuffer_WP)
  		{
  			int dmaSize = 0;
  			int dmaStart;
  			if(sendBuffer_RP < sendBuffer_WP)
  			{
  				dmaSize = sendBuffer_WP-sendBuffer_RP; // -1?? TODO
  				dmaStart = sendBuffer_RP;
  			}
  			else	// wrap around the Buffer
  			{
  				// send the rest to the end of the buffer
				dmaSize = SEND_BUFFER_SIZE - sendBuffer_RP;
				dmaStart = sendBuffer_RP;

  			}

  			if(dmaSize != 0)
  			{
  				//Restart the DMA Transfer
  				DMA_Cmd(DMA1_Channel2, DISABLE);
  				DMA_TXStructure.DMA_MemoryBaseAddr = (uint32_t) ((uint8_t*)(&sendBuffer) + dmaStart);
  				DMA_TXStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  				DMA_TXStructure.DMA_BufferSize = dmaSize;
  				DMA_Init(DMA1_Channel2, &DMA_TXStructure);
  				DMA_Cmd(DMA1_Channel2, ENABLE);
  			}
  			/*else
  			{
  				// end transfer
  				GPIO_WriteBit(GPIOB, GPIO_Pin_9, RESET);                // DE Pin low
  			}*/
  			sendBuffer_DMA = dmaStart + dmaSize;
  			if(sendBuffer_DMA == SEND_BUFFER_SIZE)
  			{
  				sendBuffer_DMA = 0;	// for wrap around
  			}
  		}
  		/*
  		else // end transfer
  		{
  			GPIO_WriteBit(GPIOB, GPIO_Pin_9, RESET);                // DE Pin low
  		}
  		*/
	}

}
