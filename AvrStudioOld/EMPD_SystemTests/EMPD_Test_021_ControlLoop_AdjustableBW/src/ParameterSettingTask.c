/*
 * ParameterSettingTask.c
 *
 * Created: 23-11-2023 13:20:17
 *  Author: rasmsmee
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <asf.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// library & HAL includes

#include "CommandConsole.h"
#include "vPrintString.h"
#include "TaskSleep.h"
#include "ADCLib.h"
#include "SwitchLib.h"
#include "map.h"

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "ApplicationTasks.h"
#include "ParameterSettingTask.h"
#include "bits.h"

///////////////////////////////////////////////////////////////////////////////
// void ParameterSettingTask(void *pvParameters)

void ParameterSettingTask(void *pvParameters)
{
	uint32_t adcData = 0;
	uint8_t  adcChannel = 0;
	uint8_t  buttonNumber = 3;
	
	double   currentWbmFactor  = -1.0;	// start with invalid value
	double   previousWbmFactor =  0.0;
	
	vPrintString("> starting ParameterSettingTask\n");

	adc_EnableChannel(adcChannel);
		
	while(true)
	{
		adc_StartConversion();
		while(adc_IsConversionReady(adcChannel) == false)
		{
			taskSleep(0);
		}

		adcData = adc_ReadData(adcChannel);
		currentWbmFactor = fmap(adcData, ADC_MIN_VALUE, ADC_MAX_VALUE, WBMFACTOR_MIN, WBMFACTOR_MAX);
		if ( fabs(currentWbmFactor - previousWbmFactor) > WBMTHRESHOLD )
		{
			xQueueOverwrite(handle_ParameterQueue, &currentWbmFactor);
			vPrintString("> wbmFactor set to %.3f\n", currentWbmFactor);
			previousWbmFactor = currentWbmFactor;
		}
		
		// pressing SW4 (buttonNumber == 3) shows the current value of the WbmFactor, 
		// it does not update the queue.
		// Show PREVIOUS value (previousWbmFactor), as it is not updated (yet)!!
		
		if (switch_IsPressed(buttonNumber))
		{
			vPrintString("> current wbmFactor = %.3f\n", previousWbmFactor); 
			// wait until button released:
			while (switch_IsPressed(buttonNumber))
			{
			}
		}

		// after first pass: let control thread know that valid 
		// data is available for use:
		xEventGroupSetBits( handle_ThreadEventGroup, BIT_1 ); 
	
		taskSleep(10);
	}
}
