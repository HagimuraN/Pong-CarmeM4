#define BALL_VELOCITY 320		//Velocity of the ball in pixels per seconds
#define WAIT_CYCLE 10			//time to wait in ms
#define TIMESCALE 1/BALL_VELOCITY

#define BALL_WIDTH 10
#define BALL_HEIGHT BALL_WIDTH
#define BALL_RADIUS (BALL_WIDTH/2)

#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 50

#define DISP_HEIGHT 240
#define DISP_WIDTH 320
#define AREA_RIGHT DISP_WIDTH-PADDLE_WIDTH
#define AREA_LEFT PADDLE_WIDTH

#define FIRST_PLAYER 0
#define SECOND_PLAYER 1

#define POT_UPPER_LIMIT 238	//upper limit that potentiometer gives.

#define POINTS_TO_WIN 10
