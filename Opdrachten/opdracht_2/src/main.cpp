/*
 * main.cpp
 *
 * Created: 13-11-2023 19:36:36
 * Author: Roel Smeets
 * Revision: V2.0
 * Modified: 07-09-2026
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <Arduino.h>
#include <string.h>
#include "dio_lib.h"

///////////////////////////////////////////////////////////////////////////////
// application includes



///////////////////////////////////////////////////////////////////////////////
// HAL includes for RTSW board

#include "led_lib.h"
#include "command_console.h"
#include "heartbeat.h"
#include "ts_printf.h"
#include "system_info.h"

///////////////////////////////////////////////////////////////////////////////
// user task handler declarations

xTaskHandle handle_UserTask		= NULL;

///////////////////////////////////////////////////////////////////////////////
// function prototypes

void HartbeatTask(void *pvParameters);
void UserTask(void *pvParameters);

void StartHartbeatTask(void);
void StartUserTasks(void);

void dirtyDelay(void);

///////////////////////////////////////////////////////////////////////////////
// void StartUserTasks(void)

void StartUserTasks(void)
{
	BaseType_t result = pdFAIL;
	uint8_t priority = 0;
	
	result = xTaskCreate(UserTask, "tsk_User", (configMINIMAL_STACK_SIZE), NULL, priority, &handle_UserTask);
	if (result == pdPASS )
	{
	}
}

void dirtyDelay(void)
{
	volatile uint32_t count = 0;
	
	// Delay for a while
	for (count = 0; count < 0xfffff; count++ )
	{
		/* This loop is just a very crude delay implementation. There is
		nothing to do in here. Later exercises will replace this crude
		loop with a proper delay function. */
	}
}


///////////////////////////////////////////////////////////////////////////////
// void UserTask(void *pvParameters)

void UserTask(void *pvParameters)
{
	dio_device dio;
	dio.Init();

	ts_printf("> UserTask started\n");

	uint8_t ledNr = 0;
	
	while (true)
	{
		ts_debug("> thread 1\n");
		dio.SetBit(ledNr);
		dirtyDelay();
		dio.ClearBit(ledNr);
		dirtyDelay();
	}
	
	// we never get here!
	vTaskDelete(NULL);
}



///////////////////////////////////////////////////////////////////////////////
// void setup(void)

void setup (void)
{
	Serial.begin(115200);
	delay(1000);
	Serial.println("System initializing");
	StartTsPrintfTask(NULL);

	StartHeartbeatTask(LED_PIN_ESP32_BOARD);
	delay(500);
	RegisterSystemInfoCommands();
	delay(500);

	StartUserTasks();
	StartCommandConsoleTask(NULL);

	delay(500);
}

void loop(void)
{
  // Do nothing, just delay to yield CPU time to other tasks
  delay(500);
}
