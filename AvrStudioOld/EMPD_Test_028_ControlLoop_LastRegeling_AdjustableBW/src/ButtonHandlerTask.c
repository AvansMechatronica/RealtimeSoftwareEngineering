/*
 * ButtonHandlerTask.c
 *
 * Created: 23-11-2023 13:11:18
 *  Author: rasmsmee
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <asf.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// FreeRTOS includes

#include "CommandConsole.h"
#include "vPrintString.h"
#include "TaskSleep.h"


///////////////////////////////////////////////////////////////////////////////
// other includes

#include "SwitchLib.h"
#include "ButtonHandlerTask.h"
#include "bits.h"
#include "ApplicationTasks.h"


///////////////////////////////////////////////////////////////////////////////
// void ButtonHandlerTask(void *pvParameters)

void ButtonHandlerTask(void *pvParameters)
{
	uint8_t buttonNumber = 0;	// 0 == SW1
	
	vPrintString("> starting ButtonHandlerTask\n");

	// signal to control thread that ButtonHandlerTask is up and running:
	xEventGroupSetBits( handle_ThreadEventGroup, BIT_0 );
	
	while(true)
	{
		if (switch_IsPressed(buttonNumber))
		{
			// wait until button released:
			while (switch_IsPressed(buttonNumber))
			{
			}
			vPrintString("> restart button SW%d pressed!\n", buttonNumber + 1);
			xSemaphoreGive(handle_RestartSemaphore);
		}
		taskSleep(10);
	}
}
