#include "functions.h"


volatile uint32_t tick = 0;
volatile uint32_t timeout = 0;
volatile uint8_t DMABufTX[100];
volatile uint8_t DMABufRX[100];

volatile uint8_t BufWrite[100];
volatile uint8_t BufRead[100];


/***************************************************************************
*  	Built-in LED configuration
*	Enabling GPIOB clock and setting pins to output mode
*  	GPIOB.0 - green		GPIOB.7	- blue		GPIOB.14 - red	
****************************************************************************/

void LedConf(void){
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER &= ~GPIO_MODER_MODER0;
	GPIOB->MODER |= GPIO_MODER_MODER0_0;
	GPIOB->MODER &= ~GPIO_MODER_MODER7;
	GPIOB->MODER |= GPIO_MODER_MODER7_0;
	GPIOB->MODER &= ~GPIO_MODER_MODER14;
	GPIOB->MODER |= GPIO_MODER_MODER14_0;
	
}

/***************************************************************************
*  	Turning LEDs on and off
*	3 states - ON/OFF/TOGGLE
****************************************************************************/

void LedOnOff(LedCol col, eLed state){
	
	switch(col){
		case red:
			if(state == LedOff) 		GPIOB->ODR &= ~GPIO_ODR_ODR_14;
			else if(state == LedOn) 	GPIOB->ODR |= GPIO_ODR_ODR_14;
			else if(state == LedTog) 	GPIOB->ODR ^= GPIO_ODR_ODR_14;
		break;
		case blue:
			if(state == LedOff) 		GPIOB->ODR &= ~GPIO_ODR_ODR_7;
			else if(state == LedOn) 	GPIOB->ODR |= GPIO_ODR_ODR_7;
			else if(state == LedTog) 	GPIOB->ODR ^= GPIO_ODR_ODR_7;
		break;
		case green:
			if(state == LedOff) 			GPIOB->ODR &= ~GPIO_ODR_ODR_0;
			else if(state == LedOn) 	GPIOB->ODR |= GPIO_ODR_ODR_0;
			else if(state == LedTog) 	GPIOB->ODR ^= GPIO_ODR_ODR_0;
		break;
	}
	
}


/***************************************************************************
*  	SysTick Handler
*	Increasing variables used to delay the programme
****************************************************************************/

void SysTick_Handler(void){
	tick++;
	timeout++;
}


/***************************************************************************
*  	Delay functions
****************************************************************************/

void delay_ms(uint32_t ms){
	tick = 0;
	while(tick<ms);
}

void delay_ms2(uint32_t ms){
	timeout = 0;
	while(timeout<ms);
}

/***************************************************************************
*  	Built-in User Button configuration
*	Enabling GPIOC clock and setting pin to input mode
****************************************************************************/

void B1Conf(void){

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER &= ~GPIO_MODER_MODE13;

}	

/***************************************************************************
*  	Built-in User Button read function
*	Returning bool value if the button is pressed or not
****************************************************************************/

bool B1Read(void){
	if((GPIOC->IDR & GPIO_IDR_ID13) != 0) return true;
	else return false;
}


/***************************************************************************
*  	USART2 Configuration using DMA
*	Enabling GPIOD, USART2, DMA1 clocks
*	Setting GPIOD.5, GPIOD.6 to alternate function mode
*	Setting Baud Rate to 19200 bps
*	Enabling DMA mode for transmission and reception
*	Setting GPIOs to proper alternate function
*	Enabling receiver, transmitter and USART
*	Enabling IDLE interrupt
*	Enabling NVIC for USART2 IRQ
*	Selecting memory source for DMA stream transmission 
*	Selecting maximum size of transfer and the array to send data from
*	Memory increment mode, direction - out, transfer complete interrupt enable
*	Selecting memory source for DMA stream reception 
*	Selecting maximum size of transfer and the array to put data into
*	Memory increment mode, direction - in, stream enable
****************************************************************************/

void USART2ConfDMA(void){
	
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	
	RCC -> AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	
	
	GPIOD->MODER &= ~GPIO_MODER_MODER6 & ~GPIO_MODER_MODER5;
	GPIOD->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1;
	
	
	USART2->BRR = 16000000/19200;
	USART2->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
	GPIOD->AFR[0] |= 0x07700000; 
	USART2->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; 
	
	USART2->CR1 |= USART_CR1_IDLEIE;
	
	NVIC_EnableIRQ(USART2_IRQn);
	
	DMA1_Stream6 -> PAR = (uint32_t)&USART2->DR;
	DMA1_Stream6 -> M0AR = (uint32_t)BufWrite;
	DMA1_Stream6 -> NDTR = (uint16_t)100;
	DMA1_Stream6 -> CR |= DMA_SxCR_CHSEL_2;
	DMA1_Stream6 -> CR |= DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;
	
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
	
	DMA1_Stream5 -> PAR = (uint32_t)&USART2->DR;
	DMA1_Stream5 -> M0AR = (uint32_t)BufRead;
	DMA1_Stream5 -> NDTR = (uint16_t)100;
	DMA1_Stream5 -> CR |= DMA_SxCR_CHSEL_2;
	
	DMA1_Stream5 -> CR |= DMA_SxCR_MINC | DMA_SxCR_EN;
	
	
	
}

/***************************************************************************
*  	DMA reinitialisation for USART2
*	Disabling the stream and clearing transfer complete flag after reception
*	Setting maximum size, enabling the stream again
****************************************************************************/

void USART2ReinitDMA(void){
	
		DMA1_Stream5 -> CR &= ~DMA_SxCR_EN;
		DMA1 -> HIFCR |= DMA_HIFCR_CTCIF5;
	
		DMA1_Stream5 -> NDTR = (uint16_t)100;
		DMA1_Stream5 -> CR |= DMA_SxCR_EN;
	
	
}

/***************************************************************************
*  	USART2 sending using DMA
*	Putting values into array to send data from
*	Disabling the stream, setting data length
*	Enabling the stream sending data
****************************************************************************/

void USART2SendDMA(const char *str, uint16_t length){
	
		for(uint16_t i=0; i<length; i++){
				BufWrite[i]=str[i];
		}
		
		DMA1_Stream6 -> CR &= ~DMA_SxCR_EN;
		DMA1_Stream6 -> NDTR = (uint16_t)length;
		DMA1_Stream6 -> CR |= DMA_SxCR_EN;
		
}

/***************************************************************************
*  	USART2 sending using DMA
*	Putting values into array to send data from
*	Disabling the stream, setting data length
*	Enabling the stream sending data
****************************************************************************/

void USART2SendDMAUINT(const uint8_t *str, uint16_t length){
	
		for(uint16_t i=0; i<length; i++){
				BufWrite[i]=str[i];
		}
		
		DMA1_Stream6 -> CR &= ~DMA_SxCR_EN;
		DMA1_Stream6 -> NDTR = (uint16_t)length;
		DMA1_Stream6 -> CR |= DMA_SxCR_EN;

}
/***************************************************************************
*  	Interrupt handler for USART2 sending stream
*	Clearing the transfer complete flag
****************************************************************************/

void DMA1_Stream6_IRQHandler(void){
	
	if((DMA1 -> HISR & DMA_HISR_TCIF6) != RESET){
		DMA1 -> HIFCR |= DMA_HIFCR_CTCIF6;
	}
	
}

/***************************************************************************
*  	USART2 interrupt handler
*	Reinitialising DMA for USART2 whenever USART2 is IDLE, 
*	making it ready to receive new data
****************************************************************************/

void USART2_IRQHandler(void){
		
		if((USART2 -> SR & USART_SR_IDLE) != RESET){
			USART2ReinitDMA();
			uint8_t temp = USART2 -> DR;	
		}
	
}


/***************************************************************************
*  	USART3 Configuration using DMA
*	Enabling GPIOD, USART3, DMA1 clocks
*	Setting GPIOD.8, GPIOD.9 to alternate function mode
*	Setting Baud Rate to 57600 bps
*	Enabling DMA mode for transmission and reception
*	Setting GPIOs to proper alternate function
*	Enabling receiver, transmitter and USART
*	Enabling IDLE interrupt
*	Enabling NVIC for USART3 IRQ
*	Selecting memory source for DMA stream transmission 
*	Selecting maximum size of transfer and the array to send data from
*	Memory increment mode, direction - out, transfer complete interrupt enable
*	Selecting memory source for DMA stream reception 
*	Selecting maximum size of transfer and the array to put data into
*	Memory increment mode, direction - in, stream enable
****************************************************************************/

void USART3ConfDMA(void)
{
		
    RCC -> APB1ENR |= RCC_APB1ENR_USART3EN;
    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIODEN;	
	RCC -> AHB1ENR |= RCC_AHB1ENR_DMA1EN;
		
    GPIOD -> MODER &= ~GPIO_MODER_MODER8 & ~GPIO_MODER_MODER9;
    GPIOD -> MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1;
		
    GPIOD -> AFR[1] |= 0x00000077;
		
    USART3 -> BRR = 16000000/57600;
		
	USART3->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
		
    USART3 -> CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
	USART3 -> CR1 |= USART_CR1_IDLEIE;
	
	NVIC_EnableIRQ(USART3_IRQn);
	
	DMA1_Stream3 -> PAR = (uint32_t)&USART3->DR;
	DMA1_Stream3 -> M0AR = (uint32_t)DMABufTX;
	DMA1_Stream3 -> NDTR = (uint16_t)100;
	DMA1_Stream3 -> CR |= DMA_SxCR_CHSEL_2;
	DMA1_Stream3 -> CR |= DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;
	
	NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	

	DMA1_Stream1 -> PAR = (uint32_t)&USART3->DR;
	DMA1_Stream1 -> M0AR = (uint32_t)DMABufRX;
	DMA1_Stream1 -> NDTR = (uint16_t)100;
	DMA1_Stream1 -> CR |= DMA_SxCR_CHSEL_2;
	DMA1_Stream1 -> CR |= DMA_SxCR_MINC | DMA_SxCR_EN;
	
	
	
}

/***************************************************************************
*  	DMA reinitialisation for USART3
*	Disabling the stream and clearing transfer complete flag after reception
*	Setting maximum size, enabling the stream again
****************************************************************************/

void USART3ReinitDMA(void){
	
		DMA1_Stream1 -> CR &= ~DMA_SxCR_EN;
		DMA1 -> LIFCR |= DMA_LIFCR_CTCIF1;
		
		DMA1_Stream1 -> NDTR = (uint16_t)100;
		DMA1_Stream1 -> CR |= DMA_SxCR_EN;
	
}

/***************************************************************************
*  	USART3 sending using DMA
*	Putting values into array to send data from
*	Disabling the stream, setting data length
*	Enabling the stream sending data
****************************************************************************/

void USART3SendDMA(const char *str, uint16_t length){
		
		for(uint16_t i=0; i<length; i++){
				DMABufTX[i]=str[i];
		}
		
		DMA1_Stream3 -> CR &= ~DMA_SxCR_EN;
		DMA1_Stream3 -> NDTR = (uint16_t)length;
		DMA1_Stream3 -> CR |= DMA_SxCR_EN;
		
}

/***************************************************************************
*  	USART3 sending using DMA
*	Putting values into array to send data from
*	Disabling the stream, setting data length
*	Enabling the stream sending data
****************************************************************************/
void USART3SendDMAUINT(const uint8_t *str, uint16_t length){
	
		for(uint16_t i=0; i<length; i++){
				DMABufTX[i]=str[i];
		}
		
		DMA1_Stream3 -> CR &= ~DMA_SxCR_EN;
		DMA1_Stream3 -> NDTR = (uint16_t)length;
		DMA1_Stream3 -> CR |= DMA_SxCR_EN;

}

/***************************************************************************
*  	Interrupt handler for USART3 sending stream
*	Clearing the transfer complete flag
****************************************************************************/

void DMA1_Stream3_IRQHandler(void){
	
	if((DMA1 -> LISR & DMA_LISR_TCIF3) != RESET){
		DMA1 -> LIFCR |= DMA_LIFCR_CTCIF3;
	}
	
}


/***************************************************************************
*  	USART2 interrupt handler
*	Reinitialising DMA for USART2 whenever USART2 is IDLE or transfer
*	has been completed, making it ready to receive new data
****************************************************************************/

void USART3_IRQHandler(void){
	
	if((USART3 -> SR & USART_SR_IDLE) != RESET){
		USART3ReinitDMA();
		uint8_t temp = USART3 -> DR;
	}
		
	if((USART3 -> SR & USART_SR_TC) != RESET){
		USART3 -> SR &= ~USART_SR_TC;
		USART3ReinitDMA();
	}
	
}

/***************************************************************************
*  	Zeroing USART3 reception array
****************************************************************************/

void ZeroDMABufRX(void){

	for(int i=0; i<50; i++){
		DMABufRX[i]=0;
	}
}














