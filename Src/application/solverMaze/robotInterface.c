#include "config/basetypes.h"
#include "config/config.h"
#include "stm32f4xx_hal.h"
/* peripherale inlcudes*/
#include "peripherals/expander/pcf8574.h"
#include "peripherals/motors/motors.h"
#include "peripherals/telemeters/telemeters.h"

/* meddleware include */
#include "middleware/controls/motionControl/mainControl.h"

/*application include */
#include "application/solverMaze/solverMaze.h"

void goOrientation(char *orientationZhonx, char directionToGo)
{
	int turn = (4 + directionToGo - *orientationZhonx) % 4;
	*orientationZhonx = directionToGo;
	switch (turn)
	{
		case FORWARD :
			break;
		case RIGHT :
			while(isEndMove() != TRUE);
			move (-90, 0, MAX_SPEED_ROTATION, 0);
			while(isEndMove() != TRUE);
			break;
		case UTURN :
			while(isEndMove() != TRUE);
			move (180, 0, MAX_SPEED_ROTATION, 0);
			while(isEndMove() != TRUE);
			break;
		case LEFT :
			while(isEndMove() != TRUE);
			move (90, 0, MAX_SPEED_ROTATION, 0);
			while(isEndMove() != TRUE);
			break;
	}
}

void move_zhonx_arc (int direction_to_go, positionRobot *positionZhonx, int numberOfCell, char end_mid_of_case, char chain)
{
	int turn = (4 + direction_to_go - positionZhonx->orientation) % 4;
	positionZhonx->orientation = direction_to_go;
	switch (turn)
	{
		case FORWARD :
			break;
		case RIGHT :
			if (positionZhonx->midOfCell == true)
			{
				while(isEndMove() != TRUE);
				move (90, 0, MAX_SPEED_ROTATION, 0);
			}
			else
			{
				moveRotateCW90(MAX_SPEED_TRANSLATION, END_SPEED_TRANSLATION);
				numberOfCell --;

			}
			break;
		case UTURN :
			if(positionZhonx->midOfCell==false)
			{
				numberOfCell --;
			}
			moveUTurn(MAX_SPEED_ROTATION, MAX_SPEED_TRANSLATION, END_SPEED_TRANSLATION);
			break;
		case LEFT :
			if (positionZhonx->midOfCell == true)
			{
				while(isEndMove() != TRUE);
				move (-90, 0, MAX_SPEED_ROTATION, 0);
			}
			else
			{
				moveRotateCCW90(MAX_SPEED_TRANSLATION, END_SPEED_TRANSLATION);
				numberOfCell --;
			}

			break;
	}
	if (positionZhonx->midOfCell == end_mid_of_case)
	{
		/*
		 * numberOfCell-=CELL_LENGTH/2;
		 * numberOfCell+=CELL_LENGTH/2;
		 */
	}
	else if (positionZhonx->midOfCell == true)
	{
		moveStartCell(MAX_SPEED_TRANSLATION, END_SPEED_TRANSLATION);
		numberOfCell --;
	}
	else // so endMidOfCase=true and positionZhonx->midOfCase=false
	{
		moveHalfCell(MAX_SPEED_TRANSLATION, END_SPEED_TRANSLATION);
	}
	moveCell (numberOfCell, MAX_SPEED_TRANSLATION, END_SPEED_TRANSLATION);
	positionZhonx->midOfCell = end_mid_of_case;

}

void doUTurn(positionRobot *positionZhonx)
{
	motorsSleepDriver(OFF);
	goOrientation (&positionZhonx->orientation,
			(positionZhonx->orientation + 2) % 4);

	move (0, -CELL_LENGTH/2, 50, 0);
	while (isEndMove() != TRUE);
	HAL_Delay(200);
	motorsSleepDriver(ON);
}

int floorSensorCalibrate(void)
{
//	int rv;
//	int i = 0;
//	unsigned long arrival_color = 30000;
//	unsigned long area_color = 500000;
//
//	lineSensorsInit ();
//	lineSensorsStart ();
//	while (1)
//	{
//		//ssd1306ClearScreen ();
//		//ssd1306Printf (0, 9, &Font_5x8, "Present arrival color");
//		//ssd1306Printf (0, 64 - 9, &Font_5x8, "'RIGHT' TO VALIDATE");
//		//ssd1306Refresh ();
//
//		arrival_color = lineSensors.front.adc_value;
//		//ssd1306Printf (10, 18, &Font_5x8, "Color sens: %i", arrival_color);
//
//		//ssd1306Refresh ();
//
//		rv = wait_validation (500);
//		if (rv == JOY_RIGHT)
//		{
//			// Value validated
//			for (i = 0; i < 100; i++)
//			{
//				arrival_color += lineSensors.front.adc_value;
//				HAL_Delay (50);
//			}
//			arrival_color /= i;
//			//ssd1306ClearScreen ();
//			//ssd1306Printf (2, 9, &Font_5x8, "Value %i validated",
//			//		arrival_color);
//			//ssd1306Refresh ();
//			HAL_Delay (1000);
//			break;
//		}
//		else if (rv == JOY_LEFT)
//		{
//			// User aborted
//			//ssd1306ClearScreen ();
//			//ssd1306Printf (2, 9, &Font_5x8, "Calibration aborted");
//			//ssd1306Refresh ();
//			HAL_Delay (1000);
//			return 0;
//		}
//	}
//
//	while (1)
//	{
//		//ssd1306ClearScreen ();
//		//ssd1306Printf (0, 9, &Font_5x8, "Present area color");
//		//ssd1306Printf (0, 64 - 9, &Font_5x8, "'RIGHT' TO VALIDATE");
//		////ssd1306Refresh ();
//
//		area_color = lineSensors.front.adc_value;
//		//ssd1306Printf (10, 18, &Font_5x8, "Color sens: %i", area_color);
//
//		//ssd1306Refresh ();
//
//		rv = wait_validation (500);
//		if (rv == JOY_RIGHT)
//		{
//			// Value validated
//			for (i = 0; i < 100; i++)
//			{
//				area_color += lineSensors.front.adc_value;
//				HAL_Delay (50);
//			}
//			area_color /= i;
//			//ssd1306ClearScreen ();
//			//ssd1306Printf (2, 9, &Font_5x8, "Value %i validated", area_color);
//			//ssd1306Refresh ();
//			HAL_Delay (1000);
//			break;
//		}
//	}
//
//	zhonxSettings.threshold_color = (MAX(arrival_color, area_color)
//			- MIN(arrival_color, area_color)) / 2;
//	//ssd1306ClearScreen ();
//	//ssd1306Printf (1, 1, &Font_5x8, "diff col : %d",
//	//		zhonxSettings.threshold_color);
//	//ssd1306Refresh ();
//	HAL_Delay (2000);
//	zhonxSettings.threshold_color += MIN(arrival_color, area_color);
//	zhonxSettings.threshold_greater = (arrival_color > area_color);
//
	return 0;
}

int waitValidation(unsigned long timeout)
{
	timeout += HAL_GetTick ();
	do
	{
		switch (expanderJoyFiltered ())
		{
			case JOY_RIGHT :
				return JOY_RIGHT;
				break;
			case JOY_LEFT :
				return JOY_LEFT;
				break;
		}
	}while (timeout > HAL_GetTick ());
	return -1;
}

void newCell(walls new_walls, labyrinthe *maze, positionRobot positionZhonx)
{
#ifdef debug
	/*print walls position*/
		ssd1306ClearRect(64,0,64,64);
		if (new_walls.front == WALL_PRESENCE)
		{
			ssd1306FillRect(64,49,54,5);
		}
		switch (new_walls.next_front)
		{
			case WALL_PRESENCE:
				ssd1306FillRect(64,0,54,5);
				break;
			case NO_KNOWN :
				ssd1306DrawRect(64,0,54,5);
				break;
			default:
				break;
		}
		switch (new_walls.left)
		{
			case WALL_PRESENCE:
				ssd1306FillRect(64,0,5,54);
				break;
			case NO_KNOWN :
				ssd1306DrawRect(64,0,5,54);
				break;
			default:
				break;
		}
		switch (new_walls.right)
		{
			case WALL_PRESENCE:
				ssd1306FillRect(113,0,5,54);
				break;
			case NO_KNOWN :
				ssd1306DrawRect(113,0,5,54);
				break;
			default:
				break;
		}
		ssd1306Refresh();
		/*end print wall position*/
#endif
//		telemetersStop();
//		motorsSleepDriver(ON);
	switch (positionZhonx.orientation)
	{
		case NORTH :
			if(positionZhonx.midOfCell == false)
			{
				maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_east = new_walls.right;
				maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_west = new_walls.left;

				if (positionZhonx.x < (MAZE_SIZE - 1))
					maze->cell[(int) (positionZhonx.x + 1)][(int) (positionZhonx.y)].wall_west = new_walls.right;
				if (positionZhonx.x > 0)
					maze->cell[(int) (positionZhonx.x - 1)][(int) (positionZhonx.y)].wall_east = new_walls.left;
			}
			if (positionZhonx.y > 0)
				maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y - 1)].wall_south = new_walls.front;

			maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_north = new_walls.front;
			break;

		case EAST :

				if(positionZhonx.midOfCell == false)
				{
					maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_south = new_walls.right;
					maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_north = new_walls.left;

					if (positionZhonx.y < (MAZE_SIZE - 1))
						maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y + 1)].wall_north = new_walls.right;
					if (positionZhonx.y > 0)
						maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y - 1)].wall_south = new_walls.left;

				}
			if (positionZhonx.x < (MAZE_SIZE - 1) )
				maze->cell[(int) (positionZhonx.x + 1)][(int) (positionZhonx.y)].wall_west = new_walls.front;
			maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_east = new_walls.front;
			break;

		case SOUTH :

			if(positionZhonx.midOfCell == false)
			{
				maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_west = new_walls.right;
				maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_east = new_walls.left;

				if (positionZhonx.x > 0)
					maze->cell[(int) (positionZhonx.x - 1)][(int) (positionZhonx.y)].wall_east = new_walls.right;
				if (positionZhonx.x < (MAZE_SIZE - 1))
					maze->cell[(int) (positionZhonx.x + 1)][(int) (positionZhonx.y)].wall_west = new_walls.left;
			}
			if (positionZhonx.y > 0)
				maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y + 1)].wall_north = new_walls.front;
			maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_south =
					new_walls.front;
			break;

		case WEST :
			if(positionZhonx.midOfCell == false)
			{
				maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_north = new_walls.right;
				maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_south = new_walls.left;


				if (positionZhonx.y > 0)
					maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y - 1)].wall_south = new_walls.right;
				if (positionZhonx.y < (MAZE_SIZE - 1))
					maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y + 1)].wall_north = new_walls.left;
			}
			if (positionZhonx.x > 0)
				maze->cell[(int) (positionZhonx.x - 1)][(int) (positionZhonx.y)].wall_east = new_walls.front;
			maze->cell[(int) (positionZhonx.x)][(int) (positionZhonx.y)].wall_west = new_walls.front;
			break;
	}
	telemetersStart();
	motorsSleepDriver(OFF);
}
