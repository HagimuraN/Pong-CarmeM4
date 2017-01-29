/**
 *****************************************************************************
 * @defgroup	Project
 * @brief		Project related code
 * @{
 *
 * @file		main.c
 * @version		1.0
 * @date		2013-11-22
 * @author		rct1
 *
 * @brief		main.c template
 *
 *****************************************************************************
 * @copyright
 * @{
 * Copyright &copy; 2013, Bern University of Applied Sciences.
 * All rights reserved.
 *
 * ##### GNU GENERAL PUBLIC LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 * @}
 *****************************************************************************
 */

/*----- Header-Files -------------------------------------------------------*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <stdbool.h>
#include <lcd.h>
#include <carme_io2.h>
#include <carme_io1.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
//#include <color.h>

#include "define.h"
#include "Draw_display.h"
#include "game.h"
#include "uart_io_init.h"
#include "uart_io_init.h"
#include "uart.h"

/*----- Macros -------------------------------------------------------------*/
#define INT_PER_SEC			1000U	/**< SysTick interrupts per second		*/
//#define WAIT_CYCLE			10		//Delay after every cycle

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
void WaitCycle(void);
void TimingDelay_Decrement(void);
int main(void);

void myfunction ();

/*----- Data ---------------------------------------------------------------*/
static __IO uint32_t time_counter;
static bool continue_cycle;
static uint8_t button_value;

unsigned int ball_coordinate_x, ball_coordinate_y;
unsigned int paddle_left_coordinate_x, paddle_left_coordinate_y;
unsigned int paddle_right_coordinate_x, paddle_right_coordinate_y;
unsigned char point_player_r = 0;
unsigned char point_player_l = 0;
/*----- Implementation -----------------------------------------------------*/
/**
 * @brief		This function waits for nTime / INT_PER_SEC seconds.
 *
 * @param[in]	nTime	Time to wait
 */
void WaitCycle(void)
{
	while (!continue_cycle);
}

/**
 * @brief		Decrement the TimingDelay. This function is called in the
 *				SysTick_Handler.
 */
void SysTick_Handler(void) {
	if (time_counter < WAIT_CYCLE)
	{
		time_counter++;
		continue_cycle=false;
	}
	else
	{
		continue_cycle=true;
		time_counter=0;
	}
}

/**
 * @brief		main
 * @return		0 if success
 */
int main(void) {
	bool gaming;	//current game mode
	bool receive_mode = FIRST_PLAYER;
	unsigned int time_counter;
	uint16_t pot_value;
	uint8_t i = 0U;
	char text[50];

	if (SysTick_Config(SystemCoreClock / INT_PER_SEC)) {
		while (1);
	}

	LCD_Init();
	CARME_IO1_Init();
	CARME_IO2_Init();
	Uart_IO_Init();
	usart_init(receive_mode);
	paddle_left_coordinate_x=0;				//position paddle
	paddle_right_coordinate_x=(DISP_WIDTH-PADDLE_WIDTH-1);
	ball_coordinate_x=(DISP_WIDTH/2);		//place the ball in middle
	ball_coordinate_y=(DISP_HEIGHT/2);
	myfunction();


	if (receive_mode == FIRST_PLAYER)
	{
		for (;;)		//Routine for Player1
		{
			i++;
			CARME_IO1_BUTTON_Get(&button_value);


			/* Read the value from the potentiometer */
			CARME_IO2_ADC_Get (CARME_IO2_ADC_PORT0, &pot_value);
			pot_value &= 0x03FF;
			pot_value /= 4;	//turn into 8bit
			paddle_left_coordinate_y=((float)pot_value/POT_UPPER_LIMIT)*(DISP_HEIGHT-PADDLE_HEIGHT);	//scale the pot-value into paddle value

			if (button_value &= 0x01) {										//Pause the game while pushing on button
				LCD_Clear(GUI_COLOR_BLACK);
				while(button_value &= 0x01){LCD_DisplayStringXY(140, 120, "Spiel Pause");}
				LCD_Clear(GUI_COLOR_BLACK);
			}
			else{
				if (gaming)
				{
				gaming = Game(&ball_coordinate_x, &ball_coordinate_y,
					 paddle_left_coordinate_x, paddle_left_coordinate_y,
					 paddle_right_coordinate_x, paddle_right_coordinate_y,
					 &point_player_r, &point_player_l);							//execute the game and save the status in variable gaming

				Draw_display(ball_coordinate_x, ball_coordinate_y,
						 paddle_left_coordinate_x, paddle_left_coordinate_y,
						 paddle_right_coordinate_x, paddle_right_coordinate_y);	//Draw the Game
				}
				else
				{
					if (time_counter <= 500)	//Display for 5s				//If someone makes a point, show "Punkt gemacht" for 5s
					{
						if (point_player_r >= POINTS_TO_WIN)
						{
							sprintf(text, "Player2 gewinnt mit %d - %d", point_player_l, point_player_r);
							LCD_DisplayStringXY(140, 120, text);
						}
						else if (point_player_l >= POINTS_TO_WIN)
						{
							sprintf(text, "Player1 gewinnt mit %d - %d", point_player_l, point_player_r);
							LCD_DisplayStringXY(140, 120, text);
						}
						else
						{
							sprintf(text, "Punkt gemacht %d - %d", point_player_l, point_player_r);
							LCD_DisplayStringXY(140, 120, text); //DELAY, dass schrift länger bleibt??
						}

						time_counter++;
					}
					else
					{
						if (point_player_r >= POINTS_TO_WIN || point_player_l >= POINTS_TO_WIN)
						{
							point_player_r = 0;
							point_player_l = 0;
						}
						gaming = true;
						time_counter = 0;
						LCD_Clear(GUI_COLOR_BLACK);
					}
				}
			}

			uart_send(ball_coordinate_x, ball_coordinate_y,						//send the coordinates
					  paddle_left_coordinate_x, paddle_left_coordinate_y,
					  paddle_right_coordinate_x, paddle_right_coordinate_y);
			uart_receive(paddle_right_coordinate_y);	//Read potmeter coordinate of the second player
			WaitCycle();
		}


	}
	else
	{
		for(;;)		//Routine for second player
		{
			/* Read the value from the potentiometer */
			CARME_IO2_ADC_Get (CARME_IO2_ADC_PORT0, &pot_value);
			pot_value &= 0x03FF;
			pot_value /= 4;		//Turn into 8-bit
			paddle_right_coordinate_y=(pot_value/0xFF)*(DISP_HEIGHT-PADDLE_HEIGHT);	//scale the pot-value into paddle value

			Draw_display(ball_coordinate_x, ball_coordinate_y,
					 	 paddle_left_coordinate_x, paddle_left_coordinate_y,
					 	 paddle_right_coordinate_x, paddle_right_coordinate_y);	//Draw the Game

			uart_send(paddle_right_coordinate_y);
			uart_receive(ball_coordinate_x, ball_coordinate_y,
						 paddle_left_coordinate_x, paddle_left_coordinate_y,
						 paddle_right_coordinate_x, paddle_right_coordinate_y);
		}
	}

	return 0U;
}
/**
 * @}
 */
