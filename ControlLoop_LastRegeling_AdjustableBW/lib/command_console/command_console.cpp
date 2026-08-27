#include <Arduino.h>
#include <cstring>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "command_console.h"

namespace
{
constexpr size_t kMaxInputSize = 1000;
constexpr UBaseType_t kMaxTasks = 32;

void printPrompt()
{
	Serial.print("> ");
}

void printCommandList()
{
	Serial.println("Available commands:");
	Serial.println("  help     Show this help");
	Serial.println("  version  Show firmware information");
	Serial.println("  tasks    Show FreeRTOS task statistics");
	Serial.println("  memory   Show ESP32 heap memory information");
	Serial.println("  echo     Echo text, for example: echo hello");
}

void printMemoryInfo()
{
	Serial.println("ESP32 heap memory:");
	Serial.printf("  Total heap:       %lu bytes\n", ESP.getHeapSize());
	Serial.printf("  Free heap:        %lu bytes\n", ESP.getFreeHeap());
	Serial.printf("  Minimum free:     %lu bytes\n", ESP.getMinFreeHeap());
	Serial.printf("  Largest block:    %lu bytes\n", ESP.getMaxAllocHeap());
}

void printTaskStats()
{
	TaskStatus_t *taskStatus = static_cast<TaskStatus_t *>(
		malloc(kMaxTasks * sizeof(TaskStatus_t)));
	if (taskStatus == nullptr)
	{
		Serial.println("Unable to allocate task statistics buffer.");
		return;
	}

	configRUN_TIME_COUNTER_TYPE totalRunTime = 0;
	const UBaseType_t taskCount = uxTaskGetSystemState(
		taskStatus, kMaxTasks, &totalRunTime);

	Serial.println("Name                 State  Prio  Stack  Runtime");
	Serial.println("--------------------------------------------------");
	for (UBaseType_t index = 0; index < taskCount; ++index)
	{
		const uint32_t runtimePercent = totalRunTime == 0
			? 0
			: static_cast<uint32_t>((100ULL * taskStatus[index].ulRunTimeCounter) / totalRunTime);

		Serial.printf("%-20s %-6c %4u %6u %6lu%%\n",
			taskStatus[index].pcTaskName,
			taskStatus[index].eCurrentState == eRunning ? 'R' :
				taskStatus[index].eCurrentState == eReady ? 'Y' :
				taskStatus[index].eCurrentState == eBlocked ? 'B' :
				taskStatus[index].eCurrentState == eSuspended ? 'S' : 'D',
			static_cast<unsigned int>(taskStatus[index].uxCurrentPriority),
			static_cast<unsigned int>(taskStatus[index].usStackHighWaterMark),
			runtimePercent);
	}

	free(taskStatus);
}

void processCommand(const char *command)
{
	if (strcmp(command, "") == 0)
	{
		return;
	}

	if (strcmp(command, "help") == 0)
	{
		printCommandList();
		return;
	}

	if (strcmp(command, "tasks") == 0 || strcmp(command, "taskstats") == 0)
	{
		printTaskStats();
		return;
	}

	if (strcmp(command, "memory") == 0 || strcmp(command, "heap") == 0)
	{
		printMemoryInfo();
		return;
	}

	if (strcmp(command, "version") == 0)
	{
		Serial.println("ControlLoop ESP32");
		Serial.println(__DATE__ " " __TIME__);
		return;
	}

	if (strncmp(command, "echo ", 5) == 0)
	{
		Serial.println(command + 5);
		return;
	}

	Serial.print("Unknown command: ");
	Serial.println(command);
	Serial.println("Type 'help' for a list of commands.");
}
}

extern "C" void StartCommandConsoleTask(void *pvParameters)
{
	(void)pvParameters;

	BaseType_t result = xTaskCreate(
		CommandConsoleTask,
		"CommandConsole",
		4096,
		nullptr,
		1,
		nullptr);

	if (result != pdPASS)
	{
		Serial.println("Unable to start command console task.");
	}
}

extern "C" void PrintfTask(void *pvParameters)
{
	(void)pvParameters;
	vTaskDelete(nullptr);
}

extern "C" void CommandConsoleTask(void *pvParameters)
{
	(void)pvParameters;
	static char input[kMaxInputSize];
	static char lastInput[kMaxInputSize];
	size_t inputIndex = 0;
	bool ignoreLineFeed = false;

	Serial.println();
	Serial.println("FreeRTOS command server");
	printCommandList();
	Serial.println();
	printPrompt();

	while (true)
	{
		if (!Serial.available())
		{
			vTaskDelay(pdMS_TO_TICKS(10));
			continue;
		}

		const int received = Serial.read();
		if (received < 0)
		{
			continue;
		}

		const char character = static_cast<char>(received);
		if (character == '\r' || (character == '\n' && !ignoreLineFeed))
		{
			ignoreLineFeed = character == '\r';
			Serial.println();
			if (inputIndex == 0)
			{
				strncpy(input, lastInput, kMaxInputSize);
			}
			input[kMaxInputSize - 1] = '\0';
			processCommand(input);
			strncpy(lastInput, input, kMaxInputSize);
			lastInput[kMaxInputSize - 1] = '\0';
			inputIndex = 0;
			input[0] = '\0';
			Serial.println();
			printPrompt();
		}
		else if (character == '\n')
		{
			ignoreLineFeed = false;
		}
		else if (character == '\b' || character == 127)
		{
			if (inputIndex > 0)
			{
				--inputIndex;
				input[inputIndex] = '\0';
				Serial.print("\b \b");
			}
		}
		else if (inputIndex < kMaxInputSize - 1)
		{
			input[inputIndex++] = character;
			input[inputIndex] = '\0';
			Serial.write(static_cast<uint8_t>(character));
		}
	}
}