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


button buttonDevice;

///////////////////////////////////////////////////////////////////////////////
// void ButtonHandlerTask(void *pvParameters)

void ButtonHandlerTask(void *pvParameters)
{
	uint8_t buttonNumber = 0;	// 0 == SW1
	
	//vPrint("> starting ButtonHandlerTask\n");
	buttonDevice.init();

	// signal to control thread that ButtonHandlerTask is up and running:
//	xEventGroupSetBits( handle_ThreadEventGroup, BIT_0 );
	
	while(true)
	{
#if 0
		if (buttonDevice.isPressed(buttonNumber))
		{
			// wait until button released:
			while (buttonDevice.isPressed(buttonNumber))
			{
			}
			Serial.printf("> restart button SW%d pressed!\n", buttonNumber + 1);
			xSemaphoreGive(handle_RestartSemaphore);
		}
#endif
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
