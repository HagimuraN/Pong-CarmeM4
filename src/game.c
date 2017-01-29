/**
 *****************************************************************************
 * @defgroup	Project
 * @brief		Project related code
 * @{
 *
 * @file		game.c
 * @version		1.0
 * @date		2017-01-07
 * @author		takan1
 *
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

#include "define.h"

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
bool Move_Ball (unsigned int *ptr_ball_coordinate_x, unsigned int *ptr_ball_coordinate_y);
void Paddle_Bounce_Detection (unsigned int ball_coordinate_x, unsigned int ball_coordinate_y,
		   	   	   	   	      unsigned int paddle_left_coordinate_x, unsigned int paddle_left_coordinate_y,
		   	   	   	   	      unsigned int paddle_right_coordinate_x, unsigned int paddle_right_coordinate_y);
bool Point_Detection (unsigned int ball_coordinate_x, unsigned int _ball_coordinate_y,
					  unsigned char *point_player_r, unsigned char *point_player_l);
int Change_Y_Velocity (unsigned int paddle_coordinate_y, unsigned int ball_coordinate_y);
	
/*----- Data ---------------------------------------------------------------*/
int ball_velocity_x = BALL_VELOCITY;	//velocity in pixels per second of x axis
int ball_velocity_y = 0;				//velocity in pixels per second of y axis
/*----- Implementation -----------------------------------------------------*/

bool Game (unsigned int *ball_coordinate_x, unsigned int *ball_coordinate_y,
		   unsigned int paddle_left_coordinate_x, unsigned int paddle_left_coordinate_y,
		   unsigned int paddle_right_coordinate_x, unsigned int paddle_right_coordinate_y,
		   unsigned char *point_player_r, unsigned char *point_player_l)
{	
	static bool gaming = true;						//it is true while the game is running
	//static bool ball_coordinate_changed = false;	//indicates wether or not the coordinate of the all has changed


	
	if (Move_Ball(ball_coordinate_x, ball_coordinate_y))
	{
		Paddle_Bounce_Detection(*ball_coordinate_x, *ball_coordinate_y,
				   paddle_left_coordinate_x, paddle_left_coordinate_y,
				   paddle_right_coordinate_x, paddle_right_coordinate_y);

		Change_Y_Velocity(*ball_coordinate_y, *ball_coordinate_y);
	}
	if (Point_Detection(*ball_coordinate_x, *ball_coordinate_y,
						point_player_r, point_player_l))
		gaming = false;


	return gaming;	//return the current game mode
}

bool Move_Ball (unsigned int *ptr_ball_coordinate_x, unsigned int *ptr_ball_coordinate_y)
	//returns true when the coordinate of the ball has changed
{
	static unsigned int counter;

	if (ball_velocity_x > 0) 		//positive direction to right 
	{
		if (counter >= (TIMESCALE / ball_velocity_x))
		{
			*ptr_ball_coordinate_x++;
			counter = 0;
			return true;
		}
		else 
			counter++;
	}
	else if (ball_velocity_x < 0)	//negative direction to right (left)
	{
		if (counter <= (TIMESCALE / ball_velocity_x))
		{
			*ptr_ball_coordinate_x--;
			counter = 0;
			return true;
		}
		else 
			counter--;
	}

	if (ball_velocity_y > 0) 		//positive direction to bottom
	{
		if (counter >= (TIMESCALE / ball_velocity_y))
		{
			*ptr_ball_coordinate_y++;
			counter = 0;
			return true;
		}
		else 
			counter++;
	}
	else if (ball_velocity_y < 0)	//negative direction to bottom (up)
	{
		if (counter <= (TIMESCALE / ball_velocity_y))
		{
			*ptr_ball_coordinate_y--;
			counter = 0;
			return true;
		}
		else
			counter--;
	}

	return false;
}

void Paddle_Bounce_Detection (unsigned int ball_coordinate_x, unsigned int ball_coordinate_y,
					   unsigned int paddle_left_coordinate_x, unsigned int paddle_left_coordinate_y,
					   unsigned int paddle_right_coordinate_x, unsigned int paddle_right_coordinate_y)
{
	if (ball_velocity_x > 0) 	//check collisions for the case when the ball moves twoards right
	{
		if  (((ball_coordinate_x + BALL_WIDTH) >= paddle_right_coordinate_x)     //check if any pixel of ball is next to the pixels of the right paddle 
		  && ((ball_coordinate_y + BALL_HEIGHT - 1) >= paddle_right_coordinate_y) //check if the pixel at the bottom of ball is at the same HEIGHT like the right paddle.
		  &&  (ball_coordinate_y <= (paddle_right_coordinate_y + PADDLE_HEIGHT - 1)))
		{
			ball_velocity_x = -1 * ball_velocity_x;	//reverse the direction
			ball_velocity_y = Change_Y_Velocity(paddle_right_coordinate_y, ball_coordinate_y); //calculate new y velocity
		}
	}
	
	if (ball_velocity_x < 0)	//check for collisions for the case when the ball moves twoards left
	{
		if (((ball_coordinate_x <= paddle_left_coordinate_x + PADDLE_WIDTH))	//same as the checking for the right paddle, just for the left baddle.
		 && ((ball_coordinate_y + BALL_HEIGHT -1) >= paddle_left_coordinate_y)
		 && (ball_coordinate_y <= (paddle_left_coordinate_y + PADDLE_HEIGHT -1)))
		{
			ball_velocity_x = -1 * ball_velocity_x; //reverse the direciton
			ball_velocity_y = Change_Y_Velocity(paddle_left_coordinate_y, ball_coordinate_y);	//calculate new y velocity
		}
	}

	if (((ball_coordinate_y + BALL_HEIGHT) == DISP_HEIGHT) && (ball_coordinate_y == 0))	//check for bottom and top bouncing.
		ball_velocity_y = -1 * ball_velocity_y;
}

bool Point_Detection (unsigned int ball_coordinate_x, unsigned int _ball_coordinate_y,
					  unsigned char *point_player_r, unsigned char *point_player_l)
	//returns true when someone has scored a point
{
	if (ball_velocity_x > 0) //check for scores when the ball is moving twoards right
	{
		if ((ball_coordinate_x + BALL_WIDTH) >= AREA_RIGHT)
		{
			*point_player_l++;
			return true;
		}
	}
	
	if (ball_velocity_x < 0) //check for scores when the ball is moving twoards left
	{
		if (ball_coordinate_x <= AREA_LEFT)
		{
			*point_player_r++;
			return true;
		}
	}

	return false;
}

int Change_Y_Velocity (unsigned int paddle_coordinate_y, unsigned int ball_coordinate_y)
//returns the new ball velocity of y axis
{
	unsigned int bcy_mid; //the coordinate of middle of ball.
	unsigned int pcy_mid; //the coordinate of middle of paddle.

	if (BALL_HEIGHT > 1)
		bcy_mid = ball_coordinate_y + BALL_HEIGHT / 2 - 1;
	else
		bcy_mid = 1;

	if (PADDLE_HEIGHT > 1)
		pcy_mid = paddle_coordinate_y + PADDLE_HEIGHT / 2 - 1;
	else
		pcy_mid = 1;

	return (BALL_VELOCITY / (PADDLE_HEIGHT - (PADDLE_HEIGHT/2))) * (bcy_mid - pcy_mid);
}
