/*
 * ParameterSettingTask.c
 *
 * Created: 23-11-2023 13:20:17
 *  Author: rasmsmee
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes


#include <string.h>
#include <math.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// library & HAL includes

//#include "CommandConsole.h"
#include "vprintf.h"
//#include "TaskSleep.h"
//#include "ADCLib.h"
//#include "SwitchLib.h"
//#include "map.h"

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "application_tasks.h"
#include "parameter_setting_task.h"
#include "bits.h"
#include "hardware_config.h"


///////////////////////////////////////////////////////////////////////////////
// void ParameterSettingTask(void *pvParameters)

void ParameterSettingTask(void *pvParameters)
{
	HardwareConfig *hardwareConfig = (HardwareConfig *)pvParameters;
	adc3208 *adc = &hardwareConfig->adc;

	uint32_t adcData = 0;
	uint8_t  adcChannel = 0;
	uint8_t  buttonNumber = 2;
	
	double   currentWblFactor  = -1.0;	// start with invalid value
	double   previousWblFactor =  0.0;
	
	vPrint("> starting ParameterSettingTask\n");
	
	while(true)
	{
		adcData = adc->readRaw(adcChannel);
		currentWblFactor = fmap(adcData, ADC_MIN_VALUE, ADC_MAX_VALUE, WBLFACTOR_MIN, WBLFACTOR_MAX);
		if ( fabs(currentWblFactor - previousWblFactor) > WBLTHRESHOLD )
		{
			xQueueOverwrite(handle_ParameterQueue, &currentWblFactor);
			vPrint("> wblFactor set to %.3f\n", currentWblFactor);
			previousWblFactor = currentWblFactor;
		}
		
		// pressing Button B2 (buttonNumber == 2) shows the current value of the WblFactor, 
		// it does not update the queue.
		// Show PREVIOUS value (previousWblFactor), as it might be not updated (yet)!!
		
		if (hardwareConfig->buttons.isPressed(buttonNumber))
		{
			vPrint("> current wblFactor = %.3f\n", previousWblFactor); 
			// wait until button released:
			while (hardwareConfig->buttons.isPressed(buttonNumber))
			{
			}
		}

		// after first pass: let control thread know that valid 
		// data is available for use:
		xEventGroupSetBits( handle_ThreadEventGroup, BIT_1 ); 
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
