/**************************************************************************/
/*!
    @file    line_follower.c
    @author   BM Pacabot.com
    @date     05 May 2015
    @version  0.00
 */
/**************************************************************************/
/* STM32 hal library declarations */
#include "stm32f4xx_hal.h"

/* General declarations */
#include "config/basetypes.h"
#include "config/config.h"
#include "config/errors.h"

#include "stdbool.h"
#include <arm_math.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/* Peripheral declarations */
#include "peripherals/lineSensors/lineSensors.h"
#include "peripherals/tone/tone.h"
#include "peripherals/display/ssd1306.h"
#include "peripherals/display/smallfonts.h"
#include "peripherals/expander/pcf8574.h"
#include "peripherals/motors/motors.h"
#include "peripherals/encoders/ie512.h"
#include "peripherals/multimeter/multimeter.h"
#include "peripherals/telemeters/telemeters.h"
#include "peripherals/bluetooth/bluetooth.h"



/* Middleware declarations */
#include "middleware/wall_sensors/wall_sensors.h"
#include "middleware/controls/pidController/pidController.h"
#include "middleware/controls/motionControl/positionControl.h"
#include "middleware/controls/motionControl/speedControl.h"
#include "middleware/controls/motionControl/transfertFunction.h"
#include "middleware/controls/motionControl/followControl.h"
#include "middleware/controls/motionControl/mainControl.h"

/* Declarations for this module */
#include "application/lineFollower/lineFollower.h"
#include "application/statistiques/statistiques.h"

line_follower_struct line_follower;
ground_sensors_struct max_Floor;
ground_sensors_struct coef_Floor;
ground_sensors_struct min_Floor;

//__IO uint16_t ADC1ConvertedValues[2] = {0};
//__IO uint16_t ADC3ConvertedValues[3] = {0};

GPIO_InitTypeDef GPIO_InitStruct;

void lineSensorsCalibration(void)
{
	mainControlInit();
	telemetersStop();
	lineSensorsInit();
	lineSensorsStart();
	motorsInit();
	motorsSleepDriver(OFF);
	control_params.line_follow_state = TRUE;

	tone(a, 500);
//	HAL_Delay(1000);
	move(0, 100, 200, 0);

// -------------------------------------------------------------
// Init line Sensor

	max_Floor.left=(double)lineSensors.left.adc_value;
	max_Floor.front=(double)lineSensors.front.adc_value;
	max_Floor.right=(double)lineSensors.right.adc_value;
	max_Floor.leftExt=(double)lineSensors.left_ext.adc_value;
	max_Floor.rightExt=(double)lineSensors.right_ext.adc_value;
	memcpy(&min_Floor, &max_Floor, sizeof(ground_sensors_struct) );
	while(isEndMove() != TRUE)
	{
		if (lineSensors.left.adc_value < min_Floor.left) min_Floor.left = lineSensors.left.adc_value;
		if (lineSensors.front.adc_value < min_Floor.front) min_Floor.front = lineSensors.front.adc_value;
		if (lineSensors.right.adc_value < min_Floor.right) min_Floor.right = lineSensors.right.adc_value;
		if (lineSensors.left_ext.adc_value < min_Floor.leftExt) min_Floor.leftExt = lineSensors.left_ext.adc_value;
		if (lineSensors.right_ext.adc_value < min_Floor.rightExt) min_Floor.rightExt = lineSensors.right_ext.adc_value;

		if (lineSensors.left.adc_value > max_Floor.left) max_Floor.left = lineSensors.left.adc_value;
		if (lineSensors.front.adc_value > max_Floor.front) max_Floor.front = lineSensors.front.adc_value;
		if (lineSensors.right.adc_value > max_Floor.right) max_Floor.right = lineSensors.right.adc_value;
		if (lineSensors.left_ext.adc_value > max_Floor.leftExt) max_Floor.leftExt = lineSensors.left_ext.adc_value;
		if (lineSensors.right_ext.adc_value > max_Floor.rightExt) max_Floor.rightExt = lineSensors.right_ext.adc_value;
	}
	tone(b, 500);
	tone(c, 500);

	// desactivate PID
	pid_loop.start_state = FALSE;
	telemetersStop();
	motorsSleepDriver(ON);
}

void lineFollower(void)
{
	mainControlInit();
	telemetersStop();
	lineSensorsInit();
	lineSensorsStart();
	motorsInit();
	follow_control.follow_type = FOLLOW_LINE;
	motorsSleepDriver(OFF);


	if (max_Floor.left-min_Floor.left< 100.0)
	{
		tone(a, 3000);
		return;
	}

	tone(c, 100);
	coef_Floor.left=1000.0/(max_Floor.left-min_Floor.left);     //  1000/(max_capteur-min_capteur)
	coef_Floor.front=1000.0/(max_Floor.front-min_Floor.front);
	coef_Floor.right=1000.0/(max_Floor.right-min_Floor.right);
	coef_Floor.leftExt=1000.0/(max_Floor.leftExt-min_Floor.leftExt);
	coef_Floor.rightExt=1000.0/(max_Floor.rightExt-min_Floor.rightExt);



	ssd1306ClearScreen();
	ssd1306PrintInt(10, 5,  "LEFT_EXT  =  ", (uint16_t) min_Floor.leftExt, &Font_5x8);
	ssd1306PrintInt(10, 15, "LEFT      =  ", (uint16_t) min_Floor.left, &Font_5x8);
	ssd1306PrintInt(10, 25, "FRONT --  =  ", (uint16_t) min_Floor.front, &Font_5x8);
	ssd1306PrintInt(10, 35, "RIGHT     =  ", (uint16_t) min_Floor.right, &Font_5x8);
	ssd1306PrintInt(10, 45, "RIGHT_EXT =  ", (uint16_t) min_Floor.rightExt, &Font_5x8);
	ssd1306Refresh();
//	HAL_Delay(900);
	tone(c, 100);

	ssd1306ClearScreen();
	ssd1306PrintInt(10, 5,  "LEFT_EXT  =  ", (uint16_t) max_Floor.leftExt, &Font_5x8);
	ssd1306PrintInt(10, 15, "LEFT      =  ", (uint16_t) max_Floor.left, &Font_5x8);
	ssd1306PrintInt(10, 25, "FRONT --  =  ", (uint16_t) max_Floor.front, &Font_5x8);
	ssd1306PrintInt(10, 35, "RIGHT     =  ", (uint16_t) max_Floor.right, &Font_5x8);
	ssd1306PrintInt(10, 45, "RIGHT_EXT =  ", (uint16_t) max_Floor.rightExt, &Font_5x8);
	ssd1306Refresh();
//	HAL_Delay(900);


//	HAL_Delay(500);

	follow_control.follow_type = FOLLOW_LINE;

	line_follower.active_state = TRUE;
	move(0, 10000, MAXSPEED, 0);
//	while(isEndMove() != TRUE);
	char marche = TRUE;
	char cpt=0;
	int  error;
	while(expanderJoyFiltered()!=JOY_LEFT && marche)
	{
		//error=follow_control.follow_error*10;
		int gauche=((double)lineSensors.left.adc_value - min_Floor.left) * coef_Floor.left ;
		int devant=((double)lineSensors.front.adc_value- min_Floor.front) * coef_Floor.front ;
		int droite=((double)lineSensors.right.adc_value- min_Floor.right) * coef_Floor.right ;
		error=line_follower.position*200;
		ssd1306ClearScreen();
//		ssd1306PrintInt(10, 5,  "LEFT_EXT  =  ", (uint16_t) lineSensors.left_ext.adc_value, &Font_5x8);
//		ssd1306PrintInt(10, 15, "LEFT      =  ", (uint16_t) lineSensors.left.adc_value, &Font_5x8);
//		ssd1306PrintInt(10, 25, "FRONT --  =  ", (uint16_t) lineSensors.front.adc_value, &Font_5x8);
//		ssd1306PrintInt(10, 35, "RIGHT     =  ", (uint16_t) lineSensors.right.adc_value, &Font_5x8);
//		ssd1306PrintInt(10, 45, "RIGHT_EXT =  ", (uint16_t) lineSensors.right_ext.adc_value, &Font_5x8);

		ssd1306PrintInt(10, 15, "LEFT      =  ", gauche, &Font_5x8);
		ssd1306PrintInt(10, 25, "FRONT --  =  ", devant, &Font_5x8);
		ssd1306PrintInt(10, 35, "RIGHT     =  ", droite, &Font_5x8);

		ssd1306PrintInt(10, 54, "Error =  ", error, &Font_5x8);
		ssd1306Refresh();


// -----------------------------------------------------------------------
// Condition to stop zhonx if no line
// -----------------------------------------------------------------------
		if ((double)lineSensors.front.adc_value < min_Floor.front *1.2 &&
			(double)lineSensors.left.adc_value < min_Floor.left *1.2 &&
			(double)lineSensors.right.adc_value < min_Floor.right *1.2)
		{
			cpt++;
			if (cpt>5)
			{
			    marche = FALSE;
			    move(0, 150, 250, 0);
			    tone(c, 500);tone(d, 500);
			}
		}
// -----------------------------------------------------------------------
// Condition to stop if right priority
// -----------------------------------------------------------------------
//		if (((double)lineSensors.left_ext.adc_value*1.2) > max_Floor.leftExt )
//		{
//			move(0, 30, 30, 0);
//			tone(c, 500);
//			// capteur telemeter ON
//			move(0, 10000, MAXSPEED, 0);
//		}

	}
	pid_loop.start_state = FALSE;
	line_follower.active_state = FALSE;
	telemetersStop();
	motorsSleepDriver(ON);
}

// fonction pour asservir zhonx sur la ligne
// -1 : ralentir
//  0 : meme vitesse
//  1 : accelerer
void asservissement(void)
{
	static maxdevant=0.0;
	line_follower.position = 0.00;
	int gauche=(lineSensors.left.adc_value - min_Floor.left) * coef_Floor.left ;
	int devant=(lineSensors.front.adc_value- min_Floor.front) * coef_Floor.front ;
	int droite=(lineSensors.right.adc_value- min_Floor.right) * coef_Floor.right ;
    int droiteExt=(lineSensors.right_ext.adc_value - min_Floor.rightExt) * coef_Floor.rightExt;
    int gaucheExt=(lineSensors.left_ext.adc_value  - min_Floor.leftExt)* coef_Floor.leftExt;

    int milieu=0.0;

	if (droite-gauche>20)
	{
		milieu=(maxdevant-devant);
	}
	else if (droite-gauche<-20)
	{
		milieu=-(maxdevant-devant);
	}else
	{
		maxdevant=devant;
	}


	line_follower.position = (double)(droite-gauche+milieu) * 0.02;

}
void lineFollower_IT(void)
{
	// Rapide
	static int vitesse=0;

	asservissement();

//	if (follow_control.follow_error > 3.0 && vitesse==0)
//	{
//		// deceleration
//		move(0, 30, MAXSPEED, MINSPEED);
//		vitesse=-1;
//	}
//	else if (follow_control.follow_error < 3.0 && vitesse==0)
//	{
//		// acceleration
//		move(0, 30, MINSPEED, MAXSPEED);
//		vitesse=1;
//	}
//
//	if (isEndMove() == TRUE)
//	{
//		if (vitesse<0)
//		{
//			move(0, 10000, MINSPEED, MINSPEED);
//		}
//		else if (vitesse>0)
//		{
//			move(0, 10000, MAXSPEED, MAXSPEED);
//		}
//		vitesse=0;
//	}
	// -----------------------------------------------------------------------
	// Condition to stop zhonx if no line
	// -----------------------------------------------------------------------
	if ((double)lineSensors.front.adc_value < min_Floor.front *1.2 &&
		(double)lineSensors.left.adc_value < min_Floor.left *1.2 &&
		(double)lineSensors.right.adc_value < min_Floor.right *1.2)
	{
	    move(0, 150, 250, 0);
	}

}

