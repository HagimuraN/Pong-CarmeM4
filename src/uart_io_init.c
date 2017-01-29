#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme_io2.h>
#include <uart.h>
#include <stdio.h>
#include <string.h>

void Uart_IO_Init (void)
{
	/***********************************************************************/
	/* Configure PA9 as USART1_TX and PA10 as USART1_RX                    */
	/***********************************************************************/

	/* The GPIOA clock Has been already initialized in the startup code    */
	/* Page 239/1718 of "RM0090 Reference Reference Manual (October 2014)" */
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

	/* Configure the GPIO port mode register as an alternate function      */
	/* Page 279/1718 of "RM0090 Reference Reference Manual (October 2014)" */
	GPIOA->MODER &= ~( GPIO_MODER_MODER9  | GPIO_MODER_MODER10 );
	GPIOA->MODER |=    GPIO_Mode_AF << 18 | GPIO_Mode_AF << 20 ;

	/* Configure Pull-up Pull down resistor configuration                  */
	/* Page 282/1718 of "RM0090 Reference Reference Manual (October 2014)" */
	GPIOA->PUPDR &= ~( GPIO_PUPDR_PUPDR9  | GPIO_PUPDR_PUPDR10 );
	GPIOA->PUPDR |=    GPIO_PuPd_UP << 18 | GPIO_PuPd_UP << 20 ;

	/* Configure the alternate function register                           */
	/* page 285/1718 of "RM0090 Reference Reference Manual (October 2014)" */
	GPIOA->AFR[1] &= ~( GPIO_AF_EVENTOUT << 4 | GPIO_AF_EVENTOUT << 8 );
	GPIOA->AFR[1] |=    GPIO_AF_USART1   << 4 | GPIO_AF_USART1   << 8 ;
}
