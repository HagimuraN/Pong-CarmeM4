/*
 * Draw_Game.c
 *
 *  Created on: Jan 11, 2017
 *      Author: Nicolas
 */


/*----- Header-Files -------------------------------------------------------*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <lcd.h>
/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/

/*----- Implementation -----------------------------------------------------*/

Draw_Game(unsigned int ball_coordinate_x, unsigned int ball_coordinate_y,
		  unsigned int paddle_left_coordinate_x, unsigned int paddle_left_coordinate_y,
		  unsigned int paddle_right_coordinate_x, unsigned int paddle_right_coordinate_y)
{
	/* Draw left paddle */
	LCD_DrawRectF(paddle_left_coordinate_x, 0, 										  PADDLE_WIDTH, paddle_left_coordinate_y, 								  GUI_COLOR_BLACK); // overwrite from top to paddle
	LCD_DrawRectF(paddle_left_coordinate_x, paddle_left_coordinate_y, 				  PADDLE_WIDTH, PADDLE_HEIGHT, 											  GUI_COLOR_RED); //draw the paddle
	LCD_DrawRectF(paddle_left_coordinate_x, paddle_left_coordinate_y + PADDLE_HEIGHT, PADDLE_WIDTH, DISP_HEIGHT - (paddle_left_coordinate_y + PADDLE_HEIGHT), GUI_COLOR_BLACK); //overwrite from bottom of paddle to bottom of screen

	/* Draw right paddle */
	LCD_DrawRectF(paddle_right_coordinate_x, 0, 										  PADDLE_WIDTH, paddle_right_coordinate_y, 								   GUI_COLOR_BLACK); // overwrite from top to paddle
	LCD_DrawRectF(paddle_right_coordinate_x, paddle_right_coordinate_y, 				  PADDLE_WIDTH, PADDLE_HEIGHT, 											   GUI_COLOR_RED); //draw the paddle
	LCD_DrawRectF(paddle_right_coordinate_x, paddle_right_coordinate_y + PADDLE_HEIGHT,   PADDLE_WIDTH, DISP_HEIGHT - (paddle_right_coordinate_y + PADDLE_HEIGHT), GUI_COLOR_BLACK); //overwrite from bottom of paddle to bottom of screen

	/* Draw the ball */
	LCD_DRAWRectF(paddle_left_coordinate_x + PADDLE_WIDTH, 0, ball_coordinate_x - (paddle_left_coordinate_x + PADDLE_WIDTH), DISP_HEIGHT, GUI_COLOR_BLACK); //overwrite the left side
	LCD_DRAWRectF(paddle_left_coordinate_x + PADDLE_WIDTH, 0, (paddle_right_coordinate_x) - (paddle_left_coordinate_x + PADDLE_WIDTH - 1), ball_coordinate_y - 1, GUI_COLOR_BLACK); //overwrite the top side
	LCD_DRAWRectF(ball_coordinate_x + BALL_WIDTH, 0, paddle_right_coordinate_x - ball_coordinate_x + BALL_WIDTH - 1, DISP_HEIGHT, GUI_COLOR_BLACK);	//overwrite the right side
}
