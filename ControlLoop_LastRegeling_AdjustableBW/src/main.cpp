/*
 * main.c
 *
 * Created: 13-11-2023 19:36:36
 * Author: Roel Smeets
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
#include "oled_lib.h"
#include "ApplicationTasks.h"
#include "vprintf.h"

///////////////////////////////////////////////////////////////////////////////
// Function prototypes

void HeartbeatTask(void *pvParameters);
void StartHeartbeatTask(void);

//void vApplicationIdleHook( void );
//void vApplicationMallocFailedHook(void);


///////////////////////////////////////////////////////////////////////////////
// file globals

static TaskHandle_t heartbeatTaskHandle = NULL;
led statusLed;
bool isLedOn = true;
#ifdef INCLUDE_OLED_DISPLAY
oledDisplay statusOled;
#endif

///////////////////////////////////////////////////////////////////////////////
// void StartHeartbeatTask(void)

void StartHeartbeatTask(void)
{
	BaseType_t result = pdFAIL;

	result = xTaskCreate(HeartbeatTask, "tsk_Heartbeat", (configMINIMAL_STACK_SIZE * 2), NULL, 1, &heartbeatTaskHandle);
	if (result == pdPASS)
	{
	}
}


///////////////////////////////////////////////////////////////////////////////
// Stack overflow hook

void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName)
{
  Serial.printf("Stack overflow in task %s\n", pcTaskName);
	while (true)
	{
		statusLed.set(LED_BLUE, isLedOn);
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
		statusLed.set(LED_BLUE, isLedOn);
		isLedOn = !isLedOn;
    delay(50);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void HeartbeatTask(void *pvParameters)

void HeartbeatTask(void *pvParameters)
{
	Serial.printf("> Heartbeat should be running, flashing onboard LED...\n");
	
	while (true)
	{
    statusLed.set(LED_BLUE, isLedOn);
    isLedOn = !isLedOn;
    delay(500);
	}
	
	/* Should never go here */
	vTaskDelete(NULL);
}


///////////////////////////////////////////////////////////////////////////////
// int main (void)

void setup (void)
{
	Serial.begin(115200);
	delay(1000);
	Serial.println("System initializing");
	Start_vPrintTask(NULL);


  // Initialize the LED before starting the heartbeat task
	statusLed.init();
#ifdef INCLUDE_OLED_DISPLAY
	bool isOledOk  = statusOled.init();
	if(!isOledOk) {
		Serial.println("OLED Init failed!");
	}
	statusOled.clear();
	statusOled.writeLine(1, "System Initializing", ALIGN_CENTER);

#endif
	StartHeartbeatTask();
	delay(500);

	StartCommandConsoleTask(NULL);
	StartApplicationTasks();

	delay(500);
#ifdef INCLUDE_OLED_DISPLAY
	statusOled.clear();
	statusOled.writeLine(1, "System Ready", ALIGN_CENTER);
#endif
}

void loop(void)
{
  // Do nothing, just delay to yield CPU time to other tasks
  delay(500);
}
