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

///////////////////////////////////////////////////////////////////////////////
// application includes



///////////////////////////////////////////////////////////////////////////////
// HAL includes for RTSW board

#include "led_lib.h"
#include "command_console.h"
#include "heartbeat.h"
#include "oled_lib.h"
#include "application_tasks.h"
#include "ts_printf.h"
#include "system_info.h"


///////////////////////////////////////////////////////////////////////////////
// file globals

static TaskHandle_t heartbeatTaskHandle = NULL;
led statusLed;
bool isLedOn = true;
#ifdef INCLUDE_OLED_DISPLAY
oledDisplay statusOled;
#endif

///////////////////////////////////////////////////////////////////////////////
// Stack overflow hook

#if 0
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName)
{
  Serial.printf("Stack overflow in task %s\n", pcTaskName);
	while (true)
	{
		statusLed.Set(LED_BLUE, isLedOn);
		isLedOn = !isLedOn;
    delay(50);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void vApplicationMallocFailedHook(void)

void vApplicationMallocFailedHook(void)
{

  Serial.printf("Malloc failed!\n");
	while (true)
	{
		statusLed.Set(LED_BLUE, isLedOn);
		isLedOn = !isLedOn;
    delay(50);
	}
}

#endif



///////////////////////////////////////////////////////////////////////////////
// void setup(void)

void setup (void)
{
	Serial.begin(115200);
	delay(1000);
	Serial.println("System initializing");
	StartTsPrintfTask(NULL);

#ifdef INCLUDE_OLED_DISPLAY
	bool isOledOk  = statusOled.Init();
	if(!isOledOk) {
		Serial.println("OLED Init failed!");
	}
	statusOled.Clear();
	statusOled.WriteLine(1, "System Initializing", ALIGN_CENTER);

#endif
	StartHeartbeatTask(LED_PIN_ESP32_BOARD);
	delay(500);

	RegisterSystemInfoCommands();
	StartApplicationTasks();

	delay(500);

	StartCommandConsoleTask(NULL);

	delay(500);
#ifdef INCLUDE_OLED_DISPLAY
	statusOled.Clear();
	char buffer[128];
	statusOled.WriteLine(0, "System Ready", ALIGN_CENTER);
	sprintf(buffer, "Build: %s", __TIMESTAMP__);
	statusOled.WriteLine(1, buffer, ALIGN_CENTER);
	const char *version = ESP.getSdkVersion();
	sprintf(buffer, "ESP32 SDK: %s", version);
	statusOled.WriteLine(2, buffer, ALIGN_CENTER);
	sprintf(buffer, "FreeRTOS: %s", tskKERNEL_VERSION_NUMBER);
	statusOled.WriteLine(3, buffer, ALIGN_CENTER);
#endif
}

void loop(void)
{
  // Do nothing, just delay to yield CPU time to other tasks
  delay(500);
}
