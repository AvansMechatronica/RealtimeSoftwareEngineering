/*
 * vprintf.c
 *
 * Created: 11-9-2022 22:43:20
 *  Author: Roel Smeets
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <Arduino.h>
#include <string.h>
#include <stdarg.h>


///////////////////////////////////////////////////////////////////////////////
// application includes

#include "vprintf.h"

///////////////////////////////////////////////////////////////////////////////
// #define's

#define MAX_PRINT_STRING_LENGTH	512
#define PRINTF_QUEUE_SIZE		32

///////////////////////////////////////////////////////////////////////////////
// external objects

xQueueHandle printfQueue;
xTaskHandle handle_PrintfTask		  = NULL;
portMUX_TYPE printfMux = portMUX_INITIALIZER_UNLOCKED;


void PrintfTask(void *pvParameters)
{
	char *p	= NULL;
	int text_length = 0;
	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	while(true)
	{
		xQueueReceive(printfQueue, &p, portMAX_DELAY);
		Serial.printf("%s", p);
		vPortFree(p);
	}
	
	/* Should never go there */
	vTaskDelete(NULL);
}

///////////////////////////////////////////////////////////////////////////////
// void StartPrintTask(void *pvParameters)

void Start_vPrintTask(void *pvParameters)
{
	char *p = NULL;
		
	printfQueue = xQueueCreate(PRINTF_QUEUE_SIZE, sizeof(p));
	
	xTaskCreate(PrintfTask, "tsk_Printf", 
				(configMINIMAL_STACK_SIZE * 2), 
				NULL, (configMAX_PRIORITIES - 2), &handle_PrintfTask);
				
}


///////////////////////////////////////////////////////////////////////////////
// void vPrintString(const char *format, ...)

void vPrint(const char *format, ...)
{
	va_list ap;
	char *p = NULL;
	int text_length = 0;

	taskENTER_CRITICAL(&printfMux);

	p = static_cast<char *>(pvPortMalloc(MAX_PRINT_STRING_LENGTH));
	if (p == NULL)
	{
		taskEXIT_CRITICAL(&printfMux);
		return;
	}
	p[0] = '\0';
	
	// Add this line to show Taskname before printed text on console
	// Add #define INCLUDE_pcTaskGetTaskName		1 in freeRTOSConfig.h
	// sprintf(p,"[%s]: ", pcTaskGetTaskName(NULL));

	text_length = strlen(p);
	
	va_start(ap, format);
	vsprintf(p + text_length, format, ap);
	va_end(ap);

#if 1
	if(printfQueue != NULL)
	{
		// queue the POINTER to the text buffer if room available, 
		// free the buffer after printing in PrintfTask()
		if(xQueueSend(printfQueue, &p, 0) == errQUEUE_FULL)
		{
			vPortFree(p);	// no room in queue, free text buffer
		}
	}
#else
	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;
	Serial.printf("%s", p);
	vPortFree(p);
#endif

	taskEXIT_CRITICAL(&printfMux);
}
