/*
 * ControlTask.c
 *
 * Created: 10-9-2023 09:48:15
 *  Author: rasmsmee
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

#include "ADCLib.h"
#include "DAC4921Lib.h"
#include "SwitchLib.h"
#include "InterruptLib.h"
#include "QC7366Lib.h"


///////////////////////////////////////////////////////////////////////////////
// motor & position controller includes

#include "MotorControl.h"
#include "PositionController.h"

///////////////////////////////////////////////////////////////////////////////
// function prototypes

void ClockInterruptHandler(uint32_t id, uint32_t mask);
void PositionControlTask(void *pvParameters);
void ParameterControlTask(void *pvParameters);

///////////////////////////////////////////////////////////////////////////////
// externals

extern QueueHandle_t handle_ControlParameterQueue;

///////////////////////////////////////////////////////////////////////////////
// semaphore to signal interrupt task

xSemaphoreHandle InterruptSemaphore = NULL;


///////////////////////////////////////////////////////////////////////////////
// clock interrupt handler, every 1 ms

uint32_t g_InterruptCount = 0;
uint32_t g_id   = 0;
uint32_t g_mask = 0;

void ClockInterruptHandler(uint32_t id, uint32_t mask)
{
	if (InterruptSemaphore != NULL)
	{
		g_InterruptCount++;
		g_id   = id;
		g_mask = mask;

		xSemaphoreGiveFromISR(InterruptSemaphore, NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
//  void PositionControlTask(void)

 void PositionControlTask(void *pvParameters)
 {
 	uint32_t maxSemCount	 = 1;
 	uint32_t initialSemCount = 0;
	uint32_t flags			 = 0;
	 
	CONTROL_STRUCT_T controlData;
	uint32_t   itemsInQueue = 0;
	TickType_t ticksToWait  = 0;
	
	uint8_t	 qcDefaultMode = 0;
	uint8_t  qcChannel	   = 0;
	mode_register_t qcModeRegister = QC_MODE_REGISTER_0;

	float dacOutputVoltage = 0.0;
	uint8_t dacChannel = 0;

	uint32_t loopcount = 0;
		
		
	vPrintString("> ControlTask started\n");
	
	memset(&controlData, 0, sizeof(CONTROL_STRUCT_T));

	motor_DisableESCONController();
	
	dacOutputVoltage = 0.0;
	for (dacChannel = 0; dacChannel < DAC_N_CHANNELS; dacChannel++)
	{
		dac_SetOutputVoltage(dacChannel, dacOutputVoltage);
	}
	
	//qcdefaultMode = MODE_QC_2 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;
	qcDefaultMode   = MODE_QC_1 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;
		
	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qc_WriteModeRegister(qcChannel, qcModeRegister, qcDefaultMode);
		qc_EnableCounter(qcChannel);
	}
		
	ctrl_InitializeConstants();
	  	
 	InterruptSemaphore = xSemaphoreCreateCounting(maxSemCount, initialSemCount);

	flags = PIO_IT_RISE_EDGE;
	interrupt_AttachHandler(ClockInterruptHandler, PIN_30, flags);

	motor_EnableESCONController();
		 
	while (true)
	{
		// waits for periodic 1 ms interrupt timer tick to unblock this thread
		
		xSemaphoreTake(InterruptSemaphore, portMAX_DELAY);
		loopcount++;
		if ((loopcount % 1000) == 0)
		{
			vPrintString(".");
		}
		
		// see if we have any parameter changes to process from the user:
		
		itemsInQueue = uxQueueMessagesWaiting(handle_ControlParameterQueue);
		if (itemsInQueue != 0)
		{
			xQueueReceive(handle_ControlParameterQueue, &controlData, ticksToWait);
			vPrintString("> ADC0/1 = %4ld, %4ld. Button = %2d (seq = %ld)\n", 
							controlData.adcData[0], controlData.adcData[1], 
							controlData.buttonCode,
							controlData.sequenceNumber);
			// do something nice with the ADC values ;-) fmap perhaps?
		}
		
		ctrl_ExecuteController();
		
		taskSleep(0); 
	 }


	/* Should never go here */
	vTaskDelete(NULL); 
 }
 
///////////////////////////////////////////////////////////////////////////////
// void ParameterControlTask(void *pvParameters)

void ParameterControlTask(void *pvParameters)
{
	CONTROL_STRUCT_T currentControlValue;
	CONTROL_STRUCT_T previousControlValue;
	uint8_t  buttonValue = 0;
	int32_t  threshold   = 5;
	uint32_t queueFreeSpace = 0;
		
	vPrintString("> ParameterControlTask started\n");

	memset(&currentControlValue,  0, sizeof(CONTROL_STRUCT_T));
	memset(&previousControlValue, 0, sizeof(CONTROL_STRUCT_T));
	
	adc_EnableChannel(0);
	adc_EnableChannel(1);
		
	while (true)
	{
		adc_StartConversion();
		
		while(adc_IsConversionReady(0) == false)
		{
			taskSleep(0);
		}

		currentControlValue.adcData[0] = adc_ReadData(0);
		currentControlValue.adcData[1] = adc_ReadData(1);
		
		if ( abs(currentControlValue.adcData[0] - previousControlValue.adcData[0]) > threshold ||
		     abs(currentControlValue.adcData[1] - previousControlValue.adcData[1]) > threshold
		   )
		{
			queueFreeSpace = uxQueueSpacesAvailable(handle_ControlParameterQueue);
			if (queueFreeSpace == 0)
			{
				vPrintString("> *** control parameter queue full !!! ***\n");
			}
		
			buttonValue = switch_GetValue();
			
			currentControlValue.buttonCode = buttonValue;
			currentControlValue.sequenceNumber++;
			xQueueSend(handle_ControlParameterQueue, &currentControlValue, portMAX_DELAY);
			previousControlValue = currentControlValue;	// copies entire struct!
		}

		taskSleep(10);
	}
	
	/* Should never go here */
	vTaskDelete(NULL);
}
