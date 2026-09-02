/*
 * ButtonHandlerTask.cpp
 *
 * Created: 23-11-2023 13:11:18
 *  Author: rasmsmee
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"


///////////////////////////////////////////////////////////////////////////////
// other includes

#include "Arduino.h"
#include "button_lib.h"
#include "button_handler_task.h"
#include "bits.h"
#include "application_tasks.h"
#include "ts_printf.h"
#include "hardware_config.h"

button restartButton;

///////////////////////////////////////////////////////////////////////////////
// void ButtonHandlerTask(void *pvParameters)

void ButtonHandlerTask(void *pvParameters)
{
	HardwareConfig *hardwareConfig = (HardwareConfig *)pvParameters;
	uint8_t restartButtonIndex = 0;	// 0 == Button 0
	
	ts_printf("> starting ButtonHandlerTask\n");


	// signal to control thread that ButtonHandlerTask is up and running:
	xEventGroupSetBits( handle_ThreadEventGroup, BIT_0 );
	
	while(true)
	{
		if (restartButton.isPressed(restartButtonIndex))
		{
			// wait until button released:
			while (restartButton.isPressed(restartButtonIndex))
			{
			}
			Serial.printf("> restart button SW%d pressed!\n", restartButtonIndex + 1);
			xSemaphoreGive(handle_RestartSemaphore);
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
