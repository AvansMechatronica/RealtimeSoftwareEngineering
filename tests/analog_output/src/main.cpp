/*
 * main.cpp
 *
 * Created: 13-11-2023 19:36:36
 * Author: Roel Smeets
 * Revision: V2.0
 * Modified: 07-09-2026
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <Arduino.h>
#include <string.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////
// application includes
#include "command_console.h"
#include "heartbeat.h"
#include "ts_printf.h"
#include "system_info.h"



///////////////////////////////////////////////////////////////////////////////
// HAL includes for RTSW board
#include "spi_lib.h"
#include "dac_4922_lib.h"

///////////////////////////////////////////////////////////////////////////////
// FreeRTOS task handle
//
// The handle is filled by xTaskCreate() when AnalogOutputTask is started. It
// can be used later to identify, suspend, resume, or delete the task.
// Initializing it to NULL makes it clear that the task does not exist before
// setup starts it.

xTaskHandle handle_UserTask		= NULL;

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
//
// These declarations make the task and startup functions available before
// their implementations below. FreeRTOS tasks use a void pointer parameter,
// even when the task does not need application-specific parameters.

void HartbeatTask(void *pvParameters);
void AnalogOutputTask(void *pvParameters);
void UserTask(void *pvParameters);

void StartHartbeatTask(void);
void StartUserTasks(void);

float GenerateTriangleWave(float phase);
float GenerateSawtoothWave(float phase);
float GenerateSquareWave(float phase);
float GenerateSineWave(float phase);



///////////////////////////////////////////////////////////////////////////////
// StartAnalogOutputTasks
//
// Creates the application's digital output task. The task runs independently
// from the Arduino loop() function under the control of the FreeRTOS
// scheduler.
//
// configMINIMAL_STACK_SIZE is sufficient for this small task because it only
// uses a DIO object, a few integer variables, and the output functions. The
// priority is deliberately kept low so system and console tasks can continue
// to run when they need processor time.

void StartAnalogOutputTasks(void)
{
	BaseType_t result = pdFAIL;
	uint8_t priority = 0;
	
	result = xTaskCreate(AnalogOutputTask, "tsk_AnalogOutput", (configMINIMAL_STACK_SIZE) * 2, NULL, priority, &handle_UserTask);
	if (result == pdPASS )
	{
		// The task was created successfully. The scheduler will call AnalogOutputTask
		// when the task receives processor time.
	}
	// When creation fails, the task handle remains NULL. There is currently no
	// recovery action here, so the application continues without AnalogOutputTask.
}


///////////////////////////////////////////////////////////////////////////////
// AnalogOutputTask
//
// Initializes the digital I/O abstraction and repeatedly toggles one output
// bit. The connected LED therefore changes state on every cycle, providing a
// simple visual indication that the task is alive and being scheduled.
//
// pvParameters is part of the standard FreeRTOS task signature. This task does
// not currently require parameters, so it is intentionally unused.

void AnalogOutputTask(void *pvParameters)
{
	spi_device spi;
	// Initialize the SPI hardware before accessing the ADC.
	spi.Init();

	dac4922 dac;
	// Initialize the ADC hardware before accessing the analog input.
	dac.Init(&spi);

	// Report that the task has started. This message uses the normal output
	// function and is therefore independent of the DEBUG build switch.
	ts_printf("> AnalogOutputTask started\n");

	// Each channel gets its own waveform shape and its own phase accumulator,
	// so the signals run independently of each other.
	float phase[N_DAC_CHANNELS] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const float phaseStep = 0.02f;    // controls the signal frequency
	const TickType_t updatePeriod = pdMS_TO_TICKS(20);

	while (true)
	{
		dac.SetOutputVoltage(0, GenerateTriangleWave(phase[0]));  // driehoek
		dac.SetOutputVoltage(1, GenerateSawtoothWave(phase[1]));  // zaagtand
		dac.SetOutputVoltage(2, GenerateSquareWave(phase[2]));    // blokgolf
		dac.SetOutputVoltage(3, GenerateSineWave(phase[3]));      // sinus

		for (uint8_t channelNr = 0; channelNr < N_DAC_CHANNELS; channelNr++)
		{
			phase[channelNr] += phaseStep;
			if (phase[channelNr] >= 1.0f)
			{
				phase[channelNr] -= 1.0f;
			}
		}

		vTaskDelay(updatePeriod);
	}
	
	// The loop is intentionally unreachable.
	// It remains as the correct cleanup operation if the loop is later changed
	// to terminate or the task receives an explicit exit condition.
	vTaskDelete(NULL);
}


///////////////////////////////////////////////////////////////////////////////
// Waveform generators
//
// Each function takes a phase in the range [0, 1) and returns the
// corresponding output voltage within the DAC's [-10V, +10V] span.

float GenerateTriangleWave(float phase)
{
	return (phase < 0.5f) ? (DAC_MIN_VOLTAGE + phase * 4.0f * (-DAC_MIN_VOLTAGE))
	                      : (DAC_MAX_VOLTAGE - (phase - 0.5f) * 4.0f * DAC_MAX_VOLTAGE);
}

float GenerateSawtoothWave(float phase)
{
	return DAC_MIN_VOLTAGE + phase * (DAC_MAX_VOLTAGE - DAC_MIN_VOLTAGE);
}

float GenerateSquareWave(float phase)
{
	return (phase < 0.5f) ? DAC_MAX_VOLTAGE : DAC_MIN_VOLTAGE;
}

float GenerateSineWave(float phase)
{
	return sinf(phase * 2.0f * PI) * (DAC_MAX_VOLTAGE / 2.0f);
}


///////////////////////////////////////////////////////////////////////////////
// setup
//
// Arduino calls setup() once during boot. The initialization order is
// important: serial output is configured first, followed by the shared output
// task, the heartbeat, system-information commands, the digital output task,
// and finally the command console. The short delays give each subsystem time
// to finish its startup before the next subsystem begins using it.

void setup (void)
{
	Serial.begin(115200);
	delay(1000);
	Serial.println("System initializing");
	// Start the task used by ts_printf and ts_debug for serialized output.
	StartTsPrintfTask(NULL);

	// Blink the board LED as a heartbeat so the system's main activity can be
	// observed independently from the LEDs controlled by AnalogOutputTask.
	StartHeartbeatTask(LED_PIN_ESP32_BOARD);
	delay(500);
	// Add commands that expose system information through the console.
	RegisterSystemInfoCommands();
	delay(500);

	// Start the application task that controls the DIO output.
	StartAnalogOutputTasks();
	
	// Start the interactive command console after the shared services exist.
	StartCommandConsoleTask(NULL);

	delay(500);
}

void loop(void)
{
	// Application work is performed by FreeRTOS tasks. Arduino still calls
	// loop(), so it uses a delay to yield CPU time instead of busy-spinning.
  delay(500);
}
