/*
 * main.c
 *
 * Created: 13-11-2023 19:36:36
 * Author: Roel Smeets
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <asf.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "CommandConsole.h"
#include "vPrintString.h"
#include "TaskSleep.h"
#include "ApplicationTasks.h"

///////////////////////////////////////////////////////////////////////////////
// HAL includes for RTSW board

#include "DeviceIOLib.h"
#include "ADCLib.h"
#include "DAC4921Lib.h"
#include "SPILib.h"
#include "LEDLib.h"
#include "SwitchLib.h"
#include "PortIOLib.h"
#include "QC7366Lib.h"
#include "InterruptLib.h"
#include "I2CLib.h"
#include "GyroFXASLib.h"
#include "StatusLED.h"

///////////////////////////////////////////////////////////////////////////////
// Function prototypes

void HartbeatTask(void *pvParameters);
void StartHartbeatTask(void);

void vApplicationIdleHook( void );
void vApplicationMallocFailedHook(void);


///////////////////////////////////////////////////////////////////////////////
// file globals

static TaskHandle_t handle_HartbeatTask = NULL;

///////////////////////////////////////////////////////////////////////////////
// void StartHartbeatTask(void)

void StartHartbeatTask(void)
{
	BaseType_t result = pdFAIL;
	
	result = xTaskCreate(HartbeatTask, "tsk_Hartbeat", (configMINIMAL_STACK_SIZE), NULL, 0, &handle_HartbeatTask);
	if (result == pdPASS )
	{
	}
}


///////////////////////////////////////////////////////////////////////////////
// void vApplicationIdleHook( void )

void vApplicationIdleHook( void )
{
	//vPrintString("> idle task\n");
	//vTaskDelay((portTickType)(configTICK_RATE_HZ * 1.0));
}


///////////////////////////////////////////////////////////////////////////////
// void vApplicationIdleHook( void )

void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName)
{
	while (true)
	{
		ioport_toggle_pin_level(PIN_STATUS_LED);
		delay_ms(50);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void vApplicationMallocFailedHook(void)

void vApplicationMallocFailedHook(void)
{
	while (true)
	{
		ioport_toggle_pin_level(PIN_STATUS_LED);
		delay_ms(50);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void HartbeatTask(void *pvParameters)

void HartbeatTask(void *pvParameters)
{
	vPrintString("> Hartbeat should be running, flashing onboard LED...\n");
	
	while (true)
	{
		// toggle led pin
		stled_Toggle();
		taskSleep(500);
	}
	
	/* Should never go here */
	vTaskDelete(NULL);
}


///////////////////////////////////////////////////////////////////////////////
// int main (void)

int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).
	sysclk_init();
	board_init();
	
	// Insert application code here, after the board has been initialized.	
	delay_ms(200);

	dio_Init();	// *** must be called first, inits all IO lines ***

	stled_Init();
	
	adc_Init();
	spi_Init();
	dac_Init();
	led_Init();
	switch_Init();
	port_Init();
	qc_Init();
	i2c_Init();

	StartCommandConsoleTask(NULL);
	
	StartHartbeatTask();
	StartApplicationTasks();
	
	vTaskStartScheduler();
	
	while (true)
	{
	}
}
