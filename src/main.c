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

#include <stdio.h>
#include <string.h>


#include "define.h"
#include "Draw_display.h"
#include "game.h"
/*----- Macros -------------------------------------------------------------*/
#define INT_PER_SEC			1000U	/**< SysTick interrupts per second		*/
//#define WAIT_CYCLE			10		//Delay after every cycle

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
void WaitCycle(void);
void TimingDelay_Decrement(void);
int main(void);

void myfunction ();
int Carme_I01_Button_S0();

/*----- Data ---------------------------------------------------------------*/
static __IO uint32_t time_counter;
static bool continue_cycle;

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
	}
}

/**
 * @brief		main
 * @return		0 if success
 */
int main(void) {
	bool gaming;	//current game mode

	uint8_t i = 0U;

	if (SysTick_Config(SystemCoreClock / INT_PER_SEC) == 1U) {
		while (1) {
		}
	}
	LCD_Init();
	CARME_IO2_Init();

	myfunction();


	for (;;)
	{
		i++;


		if (Carme_I01_Button_S0()) {
			LCD_DisplayStringXY(140, 120, "Spiel Pause");
		}
		else{
			gaming = Game(&ball_coordinate_x, &ball_coordinate_y,
				 paddle_left_coordinate_x, paddle_left_coordinate_y,
				 paddle_right_coordinate_x, paddle_right_coordinate_y,
				 &point_player_r, &point_player_l);

			if (gaming)
			{
			Draw_display(ball_coordinate_x, ball_coordinate_y,
					 paddle_left_coordinate_x, paddle_left_coordinate_y,
					 paddle_right_coordinate_x, paddle_right_coordinate_y);
			}
			else
			{
				LCD_DisplayStringXY(140, 120, "Punkt gemacht");
			}
		}
		WaitCycle();

	}

	return 0U;
}

/**
 * @}
 */
