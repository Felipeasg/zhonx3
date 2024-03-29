/*
 * settings.c
 *
 *  Created on: 3 avr. 2015
 *      Author: Colin
 */
#include "middleware/settings/settings.h"
#include "peripherals/eeprom/24lc64.h"
#include "config/basetypes.h"

#include "gpio.h"

int settingsInit (void)
{
	zhonxSettings.calibration_enabled=false;
	zhonxSettings.color_sensor_enabled=false;
	zhonxSettings.x_finish_maze=1;
	zhonxSettings.y_finish_maze=1;
	return SETTING_MODULE_E_SUCCESS;
}

// Shutdown the robot
void halt(void)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = GPIO_PIN_8;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
}
