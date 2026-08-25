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

void test_01_StatusInputTest(void);
void test_02_QuadratureCountTest(void);
void test_03_QuadratureCountIndexTest(void);
void test_04_QuadratureIndexOnlyTest(void);

void EnableESCONController(void);
void DisableESCONController(void);

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


//////////////////////////////////////////////////////////////////////////////
// #defines

#define BIT_LIMIT_LEFT		1	// bit positions in status input port
#define BIT_LIMIT_RIGHT		2
#define BIT_ATOM_ERROR		3
#define BIT_ESCON_OVERLOAD	4

#define BIT_ESCON_ENABLE	0	// bit positions in control output port
#define BIT_ESCON_POWERON	1

///////////////////////////////////////////////////////////////////////////////
// void EnableESCONController(void)
//
//	enable ESCON controller via output port bit 0

void EnableESCONController(void)
{
	port_SetBit(BIT_ESCON_ENABLE, true);
}


///////////////////////////////////////////////////////////////////////////////
// void DisableESCONController(void)
//
// disable ESCON controller via output port bit 0

void DisableESCONController(void)
{
	port_SetBit(BIT_ESCON_ENABLE, false);
}

///////////////////////////////////////////////////////////////////////////////
// void test_01_StatusInputTest(void)
//

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

///////////////////////////////////////////////////////////////////////////////
// void test_02_QuadratureCountTest(void)

void test_02_QuadratureCountTest(void)
{
	uint32_t qcCountRegister = 0;
	uint8_t  qcChannel = 0;
	uint8_t	 qcDefaultMode = 0;
	mode_register_t qcModeRegister = QC_MODE_REGISTER_0;
	
	DisableESCONController();
	
	uint8_t ledValue = 1;

	//qcdefaultMode = MODE_QC_2 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;	
	qcDefaultMode   = MODE_QC_1 | MODE_FREERUNNING | INDEX_DISABLE | INDEX_ASYNC | FILTERCLOCK_DIV_2;
	
	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qc_WriteModeRegister(qcChannel, qcModeRegister, qcDefaultMode);
		qc_EnableCounter(qcChannel);
	}

	while (true)
	{
		for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
		{
			qcCountRegister = qc_ReadCountRegister(qcChannel);
			vPrintString("channel %d: CNT = %8d\n", qcChannel, qcCountRegister);
			led_DisplayValue(ledValue++);
		}
	
		taskSleep(500);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_03_QuadratureCountIndexTest(void)

void test_03_QuadratureCountIndexTest(void)
{
	vPrintString("> Quadrature count index test started\n");

	uint32_t qcCountRegister = 0;
	uint8_t  qcChannel = 0;
	uint8_t	 qcDefaultMode = 0;
	mode_register_t qcModeRegister = QC_MODE_REGISTER_0;
	
	DisableESCONController();

	uint8_t ledValue = 1;

	//qcdefaultMode = MODE_QC_2 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;
	qcDefaultMode   = MODE_QC_4 | MODE_FREERUNNING | INDEX_DISABLE | INDEX_ASYNC | FILTERCLOCK_DIV_2;
	
	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qc_WriteModeRegister(qcChannel, qcModeRegister, qcDefaultMode);
		qc_EnableCounter(qcChannel);
	}

	while (true)
	{
		for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
		{
			qcCountRegister = qc_ReadCountRegister(qcChannel);
			vPrintString("channel %d: CNT = %8d\n", qcChannel, qcCountRegister);

			if (qc_IsIndexSet(qcChannel))
			{
				vPrintString("> channel %d: index bit is set, clearing status...\n", qcChannel);
				qc_ClearStatusRegister(qcChannel);
			}
			
			led_DisplayValue(ledValue++);
		}
		
		taskSleep(500);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_03_QuadratureCountIndexTest(void)

void test_04_QuadratureIndexOnlyTest(void)
{
	vPrintString("> Quadrature INDEX ONLY test started\n");

	uint32_t qcCountRegister = 0;
	uint8_t  qcChannel = 0;
	uint8_t	 qcDefaultMode = 0;
	mode_register_t qcModeRegister = QC_MODE_REGISTER_0;
	
	DisableESCONController();

	uint8_t ledValue = 1;

	//qcdefaultMode = MODE_QC_2 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;
	qcDefaultMode   = MODE_QC_4 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;
	
	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qc_WriteModeRegister(qcChannel, qcModeRegister, qcDefaultMode);
		qc_EnableCounter(qcChannel);
	}

	while (true)
	{
		for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
		{
			if (qc_IsIndexSet(qcChannel))
			{
				qcCountRegister = qc_ReadCountRegister(qcChannel);
				vPrintString("> channel %d: index bit is set, CNT = %08d\n", qcChannel, qcCountRegister);
				qc_ClearStatusRegister(qcChannel);
				vPrintString("> channel %d: status register cleared\n", qcChannel);
			}
			
			led_DisplayValue(ledValue++);
		}
		
		taskSleep(1);
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
		//test_02_QuadratureCountTest();
		//test_03_QuadratureCountIndexTest();
		test_04_QuadratureIndexOnlyTest();
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
