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
#include "ButtonHandlerTask.h"
#include "bits.h"
#include "ApplicationTasks.h"
#include "vprintf.h"


button restartButton;

///////////////////////////////////////////////////////////////////////////////
// void ButtonHandlerTask(void *pvParameters)

void ButtonHandlerTask(void *pvParameters)
{
	uint8_t restartButtonIndex = 0;	// 0 == SW1
	
	vPrint("> starting ButtonHandlerTask\n");
	restartButton.init();

	// signal to control thread that ButtonHandlerTask is up and running:
	// TODO: check if this is the right way to do it, or if we should use a semaphore instead of an event group
//	xEventGroupSetBits( handle_ThreadEventGroup, BIT_0 );
	
	while(true)
	{
#if 1
		if (restartButton.isPressed(restartButtonIndex))
		{
			// wait until button released:
			while (restartButton.isPressed(restartButtonIndex))
			{
			}
			Serial.printf("> restart button SW%d pressed!\n", restartButtonIndex + 1);
			// TODO: check if this is the right way to do it, or if we should use a semaphore instead of an event group
			//xSemaphoreGive(handle_RestartSemaphore);
		}
#endif
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
