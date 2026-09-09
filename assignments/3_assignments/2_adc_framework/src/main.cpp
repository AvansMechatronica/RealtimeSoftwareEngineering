/*
 * main.cpp
 *
 * Created: 13-11-2023 19:36:36
 * Author: Roel Smeets & Gerard Harkema
 * Revision: V2.0
 * Modified: 07-09-2026
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <Arduino.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// application includes
#include "command_console.h"
#include "heartbeat.h"
#include "ts_printf.h"
#include "system_info.h"



///////////////////////////////////////////////////////////////////////////////
// HAL includes for RTSW board
#include "spi_lib.h"
#include "adc_3208_lib.h"

///////////////////////////////////////////////////////////////////////////////
// FreeRTOS task handle
//
// The handle is filled by xTaskCreate() when TaskADC is started. It can be
// used later to identify, suspend, resume, or delete the task. Initializing it
// to NULL makes it clear that the task does not exist before setup starts it.

xTaskHandle handle_TaskADC		= NULL;

///////////////////////////////////////////////////////////////////////////////
// SPI and ADC instances
//
// These objects represent the SPI interface and the ADC hardware. They are
// used throughout the application to perform analog-to-digital conversions.
spi_device spi;
adc3208 adc;


///////////////////////////////////////////////////////////////////////////////
// Function prototypes
//
// These declarations make the task and startup functions available before
// their implementations below. FreeRTOS tasks use a void pointer parameter,
// even when the task does not need application-specific parameters.

void HartbeatTask(void *pvParameters);
void TaskADC(void *pvParameters);

void StartHartbeatTask(void);
void StartTaskADCs(void);


///////////////////////////////////////////////////////////////////////////////
// StartTaskADCs
//
// Creates the application's user task. The task runs independently from the
// Arduino loop() function under the control of the FreeRTOS scheduler.
//
// configMINIMAL_STACK_SIZE is sufficient for this small task because it only
// uses a DIO object, a few integer variables, and the output functions. The
// priority is deliberately kept low so system and console tasks can continue
// to run when they need processor time.

void StartTaskADCs(void)
{
	BaseType_t result = pdFAIL;
	uint8_t priority = 0;
	uint32_t adcChannel = 0;

	// TODO: counting semaphore toevoegen
	
	result = xTaskCreate(TaskADC, "tsk_ADC", (configMINIMAL_STACK_SIZE), (void*)(adcChannel), priority, &handle_TaskADC);
	if (result == pdPASS )
	{
		// The task was created successfully. The scheduler will call TaskADC
		// when the task receives processor time.
	}
	// TODO: task maken voor ADC kanaal 1
}


///////////////////////////////////////////////////////////////////////////////
// TaskADC
//
// Reads and processes ADC values from the specified channel. The task measures
// analog input signals and provides the data for further processing or display.
//
// pvParameters is part of the standard FreeRTOS task signature. This task uses
// it to receive the ADC channel number to be monitored.

void TaskADC(void *pvParameters)
{
	uint32_t adcValue = 0;
	uint32_t channel = (uint32_t)(pvParameters);
	
	ts_printf("> Task ADC started, channel %lu\n", channel);

	// TODO: gevraagde functionaliteit toevoegen voor meten en
	// weergeven van ADC kanaal 0 elke seconde
	
	// we never get here!
	vTaskDelete(NULL);
}



///////////////////////////////////////////////////////////////////////////////
// setup
//
// Arduino calls setup() once during boot. The initialization order is
// important: serial output is configured first, followed by the shared output
// task, the heartbeat, system-information commands, the user task, and finally
// the command console. The short delays give each subsystem time to finish its
// startup before the next subsystem begins using it.

void setup (void)
{
	Serial.begin(115200);
	delay(1000);
	Serial.println("System initializing");
	// Start the task used by ts_printf and ts_debug for serialized output.
	StartTsPrintfTask(NULL);

	// Blink the board LED as a heartbeat so the system's main activity can be
	// observed independently from the LED controlled by TaskADC.
	StartHeartbeatTask(LED_PIN_ESP32_BOARD);
	delay(500);
	// Add commands that expose system information through the console.
	RegisterSystemInfoCommands();
	delay(500);

	// Initialize the SPI hardware before accessing the ADC.
	spi.Init();
	// Initialize the ADC hardware before accessing the analog input.
	adc.Init(&spi);

	// Start the application task that controls the DIO output.
	StartTaskADCs();
	
	// Start the interactive command console after the shared services exist.
	StartCommandConsoleTask(NULL);

	delay(500);
}

void loop(void)
{
	// Application work is performed by FreeRTOS tasks. Arduino still calls
	// loop(), so it uses a delay to yield CPU time instead of busy-spinning.
  delay(500);
}
