/*
 * ControlTask.c
 *
 * Created: 10-9-2023 09:48:15
 *  Author: rasmsmee
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <Arduino.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "ts_printf.h"

///////////////////////////////////////////////////////////////////////////////
// HAL includes for RTSW board

//#include "DeviceIOLib.h"
//#include "InterruptLib.h"
#include "bits.h"

///////////////////////////////////////////////////////////////////////////////
// position controller & application includes

//#include "PositionControllerLoad.h"
#include "motor_control.h"
//#include "QuadratureCounters.h"
#include "button_handler_task.h"
#include "control_task.h"
#include "application_tasks.h"
#include "command_console.h"
#include "hardware_config.h"
#include "position_controller_motor.h"


///////////////////////////////////////////////////////////////////////////////
// file globals

static SemaphoreHandle_t TimerInterruptSemaphore = NULL;
static hw_timer_t *periodicTimer = NULL;
static portMUX_TYPE controlLoopStatsLock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t timerInterruptCount = 0;
static volatile uint32_t missedTimerInterruptCount = 0;
static volatile uint32_t loopCounter = 0;
static uint32_t timerIntervalUs = 0;
static uint32_t controlLoopStatsStartMs = 0;
static HardwareConfig hardwareConfig;


///////////////////////////////////////////////////////////////////////////////
// void ClockInterruptHandler(void)
//
// invoked on every hardware timer alarm

void IRAM_ATTR ClockInterruptHandler(void)
{
	portENTER_CRITICAL_ISR(&controlLoopStatsLock);
	++timerInterruptCount;
	portEXIT_CRITICAL_ISR(&controlLoopStatsLock);

	if (TimerInterruptSemaphore != NULL)
	{
		BaseType_t higherPriorityTaskWoken = pdFALSE;
		if (xSemaphoreGiveFromISR(TimerInterruptSemaphore, &higherPriorityTaskWoken) != pdTRUE)
		{
			portENTER_CRITICAL_ISR(&controlLoopStatsLock);
			++missedTimerInterruptCount;
			portEXIT_CRITICAL_ISR(&controlLoopStatsLock);
		}
		if (higherPriorityTaskWoken == pdTRUE)
		{
			portYIELD_FROM_ISR();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// bool InitializePeriodicTimer(uint32_t intervalUs)
//
// starts a hardware timer that calls ClockInterruptHandler every intervalUs

bool InitializePeriodicTimer(uint32_t intervalUs)
{
	if ((intervalUs == 0) || (periodicTimer != NULL))
	{
		return false;
	}

	if (TimerInterruptSemaphore == NULL)
	{
		TimerInterruptSemaphore = xSemaphoreCreateBinary();
		if (TimerInterruptSemaphore == NULL)
		{
			return false;
		}
	}

	// Run the timer at 1 MHz so that each alarm tick represents one microsecond.
	periodicTimer = timerBegin(1000000);
	if (periodicTimer == NULL)
	{
		vSemaphoreDelete(TimerInterruptSemaphore);
		TimerInterruptSemaphore = NULL;
		return false;
	}

	timerAttachInterrupt(periodicTimer, &ClockInterruptHandler);
	timerAlarm(periodicTimer, intervalUs, true, 0);
	timerIntervalUs = intervalUs;
	controlLoopStatsStartMs = millis();

	return true;
}

void printControlLoopStats(void);

///////////////////////////////////////////////////////////////////////////////
// void ControlTask(void *pvParameters)

void ControlTask(void *pvParameters)
{
	HardwareConfig *hardwareConfig = (HardwareConfig *)pvParameters;
	uint32_t flags = 0;
	uint32_t maxSemCount = 1;
	uint32_t initialSemCount = 0;

	EventBits_t uxBits = 0;
	BaseType_t waitForAllbits = pdTRUE;
	BaseType_t clearAllbits	  = pdFALSE;
	TickType_t ticksToWait	  = portMAX_DELAY;

	double wblFactor = 0.0;
	command_console::RegisterCommand("controlloopstats", [](const char *args) {
		(void)args;
		printControlLoopStats();
	}, "Prints the current control loop statistics");
	ts_printf("> starting ControlTask (load)\n");

	motor_initialize(hardwareConfig);
	motor_DisableESCONController();

	posctrl_initialize(hardwareConfig);


	InitializePeriodicTimer(1000);	// 1 ms interval

	ts_printf("> ControlTask waiting for helper tasks...\n");
	// wait for ButtonHandlerTask and ParameterSettingTask to get up and running:
	uxBits = xEventGroupWaitBits(handle_ThreadEventGroup, BIT_1 | BIT_0,
								 clearAllbits, waitForAllbits, ticksToWait);	
	ts_printf("> helper tasks running, ControlTask started, event group = 0x%04x\n", uxBits);
	
	
	motor_EnableESCONController(); 
	motor_GotoHomePosition(MOVE_LEFT); 

	hardwareConfig->qc.clearCountRegister(QC_CHANNEL_0);
	hardwareConfig->qc.clearCountRegister(QC_CHANNEL_1);

	ts_printf("> ready\n");
	ts_printf("> press button SW1 to start (watch your fingers...)\n");
	
	ticksToWait = portMAX_DELAY;
	xSemaphoreTake(handle_RestartSemaphore, ticksToWait);	// wait for SW1 first button press

	while (true)
	{
		motor_GotoHomePosition(MOVE_LEFT);

		hardwareConfig->qc.clearCountRegister(QC_CHANNEL_0);
		hardwareConfig->qc.clearCountRegister(QC_CHANNEL_1);
		ts_printf("> HOME");
		
		// always leave parameter value in queue! So use xQueuePeek
		ticksToWait = 0;
		xQueuePeek(handle_ParameterQueue, &wblFactor, ticksToWait);
		ts_printf("> running with wblFactor: %.3f\n", wblFactor);
		posctrl_InitParameters(wblFactor);
		ControlLoop();	// this loop exits by pressing button SW1
	}
	
	/* Should never go here */
	vTaskDelete(NULL);
}

///////////////////////////////////////////////////////////////////////////////
//  void ControlLoop(void)

void ControlLoop(void)
{
	ts_printf("> enter Control Loop (load)\n");

	BaseType_t restart = pdFALSE;
	BaseType_t ticksToWait = 0;
	bool continueControlLoop = true;
	
	while (continueControlLoop)
	{
		// wait for periodic 1 ms timer tick to unblock this thread and
		// run the motion controller:
		xSemaphoreTake(TimerInterruptSemaphore, portMAX_DELAY);
		posctrl_RunController_MotorSide();
		
		// check restart semaphore here, invoked by button press
		restart = xSemaphoreTake(handle_RestartSemaphore, ticksToWait);
		if (restart == pdTRUE)
		{
			continueControlLoop = false;
		}
		portENTER_CRITICAL(&controlLoopStatsLock);
		++loopCounter;
		portEXIT_CRITICAL(&controlLoopStatsLock);
		vTaskDelay(0);
	}

	ts_printf("> exit Control Loop (load)\n");
}

void printControlLoopStats(void)
{
	uint32_t timerInterrupts = 0;
	uint32_t missedTimerInterrupts = 0;
	uint32_t completedLoops = 0;
	uint32_t intervalUs = 0;
	uint32_t startMs = 0;

	portENTER_CRITICAL(&controlLoopStatsLock);
	timerInterrupts = timerInterruptCount;
	missedTimerInterrupts = missedTimerInterruptCount;
	completedLoops = loopCounter;
	intervalUs = timerIntervalUs;
	startMs = controlLoopStatsStartMs;
	portEXIT_CRITICAL(&controlLoopStatsLock);

	uint32_t elapsedMs = millis() - startMs;
	uint32_t loopRateHz = elapsedMs == 0 ? 0 : static_cast<uint32_t>((static_cast<uint64_t>(completedLoops) * 1000U) / elapsedMs);

	ts_printf("> Control loop statistics:\n");
	ts_printf("  Timer interval: %lu us\n", static_cast<unsigned long>(intervalUs));
	ts_printf("  Timer interrupts: %lu\n", static_cast<unsigned long>(timerInterrupts));
	ts_printf("  Completed loops: %lu\n", static_cast<unsigned long>(completedLoops));
	ts_printf("  Coalesced interrupts: %lu\n", static_cast<unsigned long>(missedTimerInterrupts));
	ts_printf("  Elapsed time: %lu ms\n", static_cast<unsigned long>(elapsedMs));
	ts_printf("  Observed loop rate: %lu Hz\n", static_cast<unsigned long>(loopRateHz));
}
