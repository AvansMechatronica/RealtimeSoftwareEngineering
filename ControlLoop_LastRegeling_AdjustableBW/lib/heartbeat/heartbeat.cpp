/*
 * heartbeat.cpp
 *
 * Revision: V2.0
 * Modified: 07-09-2026
 */ 

#include "heartbeat.h"
#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ts_printf.h"

static TaskHandle_t heartbeatTaskHandle = NULL;

///////////////////////////////////////////////////////////////////////////////
// void HeartbeatTask(void *pvParameters)

void HeartbeatTask(void *pvParameters)
{
    bool isLedOn = true;
    unsigned int pin = (unsigned int)(uintptr_t)pvParameters;
    pinMode(pin, OUTPUT);

	ts_printf("> Heartbeat should be running, flashing onboard LED, pin %d...\n", pin);
	
	while (true)
	{
        digitalWrite(pin, isLedOn);
        isLedOn = !isLedOn;
        delay(500);

	}
	
	/* Should never go here */
	vTaskDelete(NULL);
}

///////////////////////////////////////////////////////////////////////////////
// void StartHeartbeatTask(void)

void StartHeartbeatTask(unsigned int pin)
{
	BaseType_t result = pdFAIL;


	result = xTaskCreate(HeartbeatTask, "tsk_Heartbeat", (configMINIMAL_STACK_SIZE * 2), (void *)(uintptr_t)pin, 1, &heartbeatTaskHandle);
	if (result == pdPASS)
	{
	}
}