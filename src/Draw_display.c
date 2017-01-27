#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <lcd.h>
#include <carme_io1.h>
#include <carme_io2.h>
#include <uart.h>
#include <define.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void Draw_display(unsigned int ball_coordinate_x, unsigned int ball_coordinate_y,
		  unsigned int paddle_left_coordinate_x, unsigned int paddle_left_coordinate_y,
		  unsigned int paddle_right_coordinate_x, unsigned int paddle_right_coordinate_y)
		  {

			int i;

			for(i=-BALL_RADIUS; i>= 3; i++)
			{
				if(i<=0)
				{
					LCD_DrawCircle(ball_coordinate_x+BALL_RADIUS, ball_coordinate_y+BALL_RADIUS, i+BALL_RADIUS, GUI_COLOR_RED);
				}
				else
				{
					LCD_DrawCircle(ball_coordinate_x+BALL_RADIUS, ball_coordinate_y+BALL_RADIUS, i+BALL_RADIUS, GUI_COLOR_BLACK);
				}
				/* Draw left paddle */
				LCD_DrawRectF(paddle_left_coordinate_x, 0, 										  PADDLE_WIDTH, paddle_left_coordinate_y, 								  GUI_COLOR_BLACK); // overwrite from top to paddle
				LCD_DrawRectF(paddle_left_coordinate_x, paddle_left_coordinate_y, 				  PADDLE_WIDTH, PADDLE_HEIGHT, 											  GUI_COLOR_GREEN); //draw the paddle
				LCD_DrawRectF(paddle_left_coordinate_x, paddle_left_coordinate_y + PADDLE_HEIGHT, PADDLE_WIDTH, DISP_HEIGHT - (paddle_left_coordinate_y + PADDLE_HEIGHT), GUI_COLOR_BLACK); //overwrite from bottom of paddle to bottom of screen

				/* Draw right paddle */
				LCD_DrawRectF(paddle_right_coordinate_x, 0, 										  PADDLE_WIDTH, paddle_right_coordinate_y, 								   GUI_COLOR_BLACK); // overwrite from top to paddle
				LCD_DrawRectF(paddle_right_coordinate_x, paddle_right_coordinate_y, 				  PADDLE_WIDTH, PADDLE_HEIGHT, 											   GUI_COLOR_YELLOW); //draw the paddle
				LCD_DrawRectF(paddle_right_coordinate_x, paddle_right_coordinate_y + PADDLE_HEIGHT,   PADDLE_WIDTH, DISP_HEIGHT - (paddle_right_coordinate_y + PADDLE_HEIGHT), GUI_COLOR_BLACK); //overwrite from bottom of paddle to bottom of screen
			}
		  }
