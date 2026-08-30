#include <Arduino.h>
#include <cstring>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//#include <esp_additions/freertos/task_snapshot.h>

#include "command_console.h"
#include "vprintf.h"
#include "system_info.h"



#define MAX_TASKS kMaxTasks
constexpr size_t kMaxInputSize = 1000;
constexpr UBaseType_t kMaxTasks = 32;

void printPrompt()
{
	vPrint("> ");
}

void printCommandList()
{
	vPrint("Available commands:\n");
	vPrint("  help     Show this help\n");
	vPrint("  version  Show firmware information\n");
	vPrint("  cpuinfo  Show ESP32 CPU/chip information\n");
	vPrint("  tasks    Show current task snapshot\n");
	vPrint("  taskstats Show command console task details\n");
	vPrint("  memory   Show ESP32 heap memory information\n");
	vPrint("  heap     Alias for memory\n");
	vPrint("  echo     Echo text, for example: echo hello\n");
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

	if (strcmp(command, "taskstats") == 0)
	{
		printTasksInfo();
		return;
	}

	if (strcmp(command, "tasks") == 0)
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
		printVersion();
		return;
	}

	if (strcmp(command, "cpuinfo") == 0)
	{
		printCPUInfo();
		return;
	}

	if (strncmp(command, "echo ", 5) == 0)
	{
		vPrint(command + 5);
		return;
	}

	vPrint("Unknown command: ");
	vPrint(command);
	vPrint("Type 'help' for a list of commands.");
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
		vPrint("Unable to start command console task.");
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

	//vPrint();
	vPrint("FreeRTOS command server");
	printCommandList();
	//vPrint();
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
//			vPrint();
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
//			vPrint();
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
				vPrint("\b \b");
			}
		}
		else if (inputIndex < kMaxInputSize - 1)
		{
			input[inputIndex++] = character;
			input[inputIndex] = '\0';
			vPrint("%c", character);
			//Serial.write(static_cast<uint8_t>(character));
		}
	}
}