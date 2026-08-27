

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
#if 0
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
#endif

///////////////////////////////////////////////////////////////////////////////
// Function prototypes

void HartbeatTask(void *pvParameters);
void StartHartbeatTask(void);

//void vApplicationIdleHook( void );
//void vApplicationMallocFailedHook(void);


///////////////////////////////////////////////////////////////////////////////
// file globals

static TaskHandle_t handle_HartbeatTask = NULL;
led myLed;
bool ledOn = true;

///////////////////////////////////////////////////////////////////////////////
// void StartHartbeatTask(void)

void StartHartbeatTask(void)
{
	BaseType_t result = pdFAIL;

	result = xTaskCreate(HartbeatTask, "tsk_Hartbeat", (configMINIMAL_STACK_SIZE * 2), NULL, 1, &handle_HartbeatTask);
	if (result == pdPASS)
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
  Serial.printf("Stack overflow in task %s\n", pcTaskName);
	while (true)
	{
    myLed.set(LED_IO15, ledOn);
    ledOn = !ledOn;
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
    myLed.set(LED_IO15, ledOn);
    ledOn = !ledOn;
    delay(50);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void HartbeatTask(void *pvParameters)

void HartbeatTask(void *pvParameters)
{
	Serial.printf("> Hartbeat should be running, flashing onboard LED...\n");
	
	while (true)
	{
    myLed.set(LED_IO15, ledOn);
    ledOn = !ledOn;
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
	delay(100);

	// Insert system clock initialization code here (sysclk_init()).
#if 0
	sysclk_init();
	board_init();
#endif

	// Insert application code here, after the board has been initialized.
#if 0	
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
#endif


  // Initialize the LED before starting the heartbeat task
	myLed.init();
	StartHartbeatTask();
	delay(500);
	StartCommandConsoleTask(NULL);
	//StartApplicationTasks();
}

void loop(void)
{
  // Do nothing, just delay to yield CPU time to other tasks
  delay(500);
}
