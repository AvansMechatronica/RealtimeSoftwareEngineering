/*
 * ApplicationTasks.c
 *
 * Created: 27-11-2023 14:57:44
 *  Author: rasmsmee
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <Arduino.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// application includes



#include "ApplicationTasks.h"
#include "ButtonHandlerTask.h"


///////////////////////////////////////////////////////////////////////////////
// application tasks handler declarations

static TaskHandle_t controlTaskHandle = NULL;
static TaskHandle_t buttonHandlerTaskHandle = NULL;
static TaskHandle_t parameterSettingTaskHandle = NULL;


///////////////////////////////////////////////////////////////////////////////
// global handles & objects

EventGroupHandle_t	handle_ThreadEventGroup = NULL;
SemaphoreHandle_t	handle_RestartSemaphore = NULL;
QueueHandle_t		handle_ParameterQueue	= NULL;

///////////////////////////////////////////////////////////////////////////////
// void StartApplicationTasks(void)

void StartApplicationTasks(void)
{
	BaseType_t result = pdFAIL;
	UBaseType_t parameterQueueSize = 1;

	// queue for bandwidth parameter passing
	handle_ParameterQueue = xQueueCreate(parameterQueueSize, sizeof(double));
	if (handle_ParameterQueue == NULL)
	{
	}

	// restart button semaphore
	handle_RestartSemaphore = xSemaphoreCreateBinary();
	if (handle_RestartSemaphore == NULL)
	{
	}
	
	// event group to wait for parameter setting task & button handler task before
	// running the control task on startup
	handle_ThreadEventGroup = xEventGroupCreate();
	if (handle_ThreadEventGroup == NULL)
	{
	}

#if 0
	result = xTaskCreate(ControlTask, "tsk_Control", (configMINIMAL_STACK_SIZE), NULL, 0, &controlTaskHandle);
	if (result == pdPASS )
	{
	}
#endif
#if 1
	result = xTaskCreate(ButtonHandlerTask, "tsk_Button", (configMINIMAL_STACK_SIZE), NULL, 0, &buttonHandlerTaskHandle);
	if (result == pdPASS )
	{
	}
#endif
#if 0
	result = xTaskCreate(ParameterSettingTask, "tsk_ParamHandler", (configMINIMAL_STACK_SIZE), NULL, 0, &parameterSettingTaskHandle);
	if (result == pdPASS )
	{
	}
#endif
}

