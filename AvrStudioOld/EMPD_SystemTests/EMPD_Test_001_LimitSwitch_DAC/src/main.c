/*
 * main.c
 *
 * Created: 16-11-2022 19:36:36
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
// Task handler declarations
 
xTaskHandle handle_HartbeatTask		= NULL;
xTaskHandle handle_ApplicationTask	= NULL;


void HartbeatTask(void *pvParameters);
void ApplicationTask(void *pvParameters);

void StartHartbeatTask(void);
void StartApplicationTask(void);

void vApplicationIdleHook( void );
void vApplicationMallocFailedHook(void);

void interruptHandler(uint32_t id, uint32_t mask);

void DisplayStatus(void);

void test_01_StatusInputTest(void);
void test_02_DACTest_Serial(void);
void test_03_DACTest_Parallel(void);
void test_04_DACTest_SetOutputVoltage(void);
void test_05_DACTest_ToggleOutputVoltage(void);

#define BIT_LIMIT_LEFT		1	// bit positions in input port
#define BIT_LIMIT_RIGHT		2
#define BIT_ATOM_ERROR		3
#define BIT_ESCON_OVERLOAD	4


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
// void StartApplicationTask(void)

void StartApplicationTask(void)
{
	BaseType_t result = pdFAIL;
	
	result = xTaskCreate(ApplicationTask, "tsk_Application", (configMINIMAL_STACK_SIZE), NULL, 0, &handle_ApplicationTask);
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

void DisplayStatus(void)
{
	uint8_t portInValue = 0;
	uint8_t bitVal = 0;
		
	portInValue = port_GetInput();
			
	led_DisplayValue(portInValue >> 0x01);

	vPrintString("digital input = 0x%02x\n", portInValue);
			
	bitVal = ((portInValue & _BV(BIT_LIMIT_LEFT)) == 0) ? 0 : 1;
	vPrintString("Limit Left:     %d\n", bitVal);

	bitVal = ((portInValue & _BV(BIT_LIMIT_RIGHT)) == 0) ? 0 : 1;
	vPrintString("Limit Right:    %d\n", bitVal);

	bitVal = ((portInValue & _BV(BIT_ATOM_ERROR)) == 0) ? 0 : 1;
	vPrintString("Atom Error:     %d\n", bitVal);

	bitVal = ((portInValue & _BV(BIT_ESCON_OVERLOAD)) == 0) ? 0 : 1;
	vPrintString("ESCON Overload: %d\n", bitVal);

	vPrintString("\n");
}

///////////////////////////////////////////////////////////////////////////////
// void test_01_StatusInputTest(void)

void test_01_StatusInputTest(void)
{
	uint8_t portInValue = 0;
	uint8_t bitVal = 0;
	
	while (true)
	{
		portInValue = port_GetInput();
		
		led_DisplayValue(portInValue >> 0x01);

		vPrintString("digital input = 0x%02x\n", portInValue);
		
		bitVal = ((portInValue & _BV(BIT_LIMIT_LEFT)) == 0) ? 0 : 1;
		vPrintString("Limit Left:     %d\n", bitVal);

		bitVal = ((portInValue & _BV(BIT_LIMIT_RIGHT)) == 0) ? 0 : 1;
		vPrintString("Limit Right:    %d\n", bitVal);

		bitVal = ((portInValue & _BV(BIT_ATOM_ERROR)) == 0) ? 0 : 1;
		vPrintString("Atom Error:     %d\n", bitVal);

		bitVal = ((portInValue & _BV(BIT_ESCON_OVERLOAD)) == 0) ? 0 : 1;
		vPrintString("ESCON Overload: %d\n", bitVal);

		vPrintString("\n");
		
		taskSleep(500);
	}
}

void test_02_DACTest_Serial(void)
{
	uint32_t dacValue	= 0;
	uint8_t  dacChannel = 0;
	uint8_t	 ledValue	= 0;
	uint32_t status = 0;
	
	while (true)
	{
		for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++ )
		{
			for (dacValue = 0; dacValue <= DAC_MAX_VALUE; dacValue++)
			{
				status = dac_Write(dacChannel, dacValue);
			}
		}
		
		led_DisplayValue(ledValue++);
	}
}


void test_03_DACTest_Parallel(void)
{
	uint32_t dacValue	= 0;
	uint8_t	 ledValue	= 0;
	
	while (true)
	{
		for (dacValue = 0; dacValue <= DAC_MAX_VALUE; dacValue++)
		{
			dac_WriteAll(dacValue);
		}
		
		led_DisplayValue(ledValue++);
	}
}


void test_04_DACTest_SetOutputVoltage(void)
{
	uint8_t switchValue = 0;
	uint8_t dacChannel  = 0;
	float dacOutputVoltage = 0.0;
	
	while (true)
	{
		switchValue = switch_GetValue();
		led_DisplayValue(switchValue);
		
		switch (switchValue)
		{
			case 0:
			dacOutputVoltage = 0.0;
			break;
			
			case 1:
			dacOutputVoltage = 9.5;
			break;
			
			case 2:
			dacOutputVoltage = -9.5;
			break;
			
			case 4:
			dacOutputVoltage = 5.0;
			break;

			case 8:
			dacOutputVoltage = -5.0;
			break;
			
			default:
			dacOutputVoltage = 0.0;
			break;
		}

		for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++)
		{
			dac_SetOutputVoltage(dacChannel, dacOutputVoltage);
		}

	}
}

void test_05_DACTest_ToggleOutputVoltage(void)
{
	uint8_t dacChannel  = 0;
	float dacOutputVoltage_1 = 2.0;
	float dacOutputVoltage_2 = -2.0;
	
	while (true)
	{
		DisplayStatus();
		
		for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++)
		{
			dac_SetOutputVoltage(dacChannel, dacOutputVoltage_1);
		}

		taskSleep(1000);
		
		for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++)
		{
			dac_SetOutputVoltage(dacChannel, 0.0);
		}

		taskSleep(1000);
				
		for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++)
		{
			dac_SetOutputVoltage(dacChannel, dacOutputVoltage_2);
		}

		taskSleep(1000);
		
		for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++)
		{
			dac_SetOutputVoltage(dacChannel, 0.0);
		}

		taskSleep(1000);

	}
}

/*

uint32_t int_pin = 0;
uint32_t g_InterruptCount = 0;
uint32_t g_id  = 0;
uint32_t g_mask = 0;

void interruptHandler(uint32_t id, uint32_t mask)
{
	g_InterruptCount++;
	g_id = id;
	g_mask = mask;
}

void test_07_InterruptTest(void)
{
	uint32_t flags = PIO_IT_FALL_EDGE | PIO_DEBOUNCE;
	
	interrupt_AttachHandler(interruptHandler, PIN_A6, flags);
	interrupt_AttachHandler(interruptHandler, PIN_A7, flags);
	interrupt_AttachHandler(interruptHandler, PIN_A8, flags);
	interrupt_AttachHandler(interruptHandler, PIN_A9, flags);
	
	g_InterruptCount = 0;
	g_id = 0;
	g_mask = 0;
	
	while (true)
	{
		vPrintString("CNT = %lu, ID = %d, mask = 0x%08x\n", g_InterruptCount, g_id, g_mask);
		taskSleep(500);
	}
		
}
*/


///////////////////////////////////////////////////////////////////////////////
// void ApplicationTask(void *pvParameters)

void ApplicationTask(void *pvParameters)
{
	vPrintString("> ApplicationTask started\n");
	
	while (true)
	{
		//test_01_StatusInputTest();
		//test_02_DACTest_Serial();
		//test_03_DACTest_Parallel();
		//test_04_DACTest_SetOutputVoltage();
		test_05_DACTest_ToggleOutputVoltage();
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
	StartApplicationTask();
	
	vTaskStartScheduler();
	
	while (true)
	{
	}
}
