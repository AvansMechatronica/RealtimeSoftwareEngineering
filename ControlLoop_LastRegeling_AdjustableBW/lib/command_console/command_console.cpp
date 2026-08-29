#include <Arduino.h>
#include <cstring>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_additions/freertos/task_snapshot.h>

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
	Serial.println("  cpuinfo  Show ESP32 CPU/chip information");
	Serial.println("  tasks    Show current task snapshot");
	Serial.println("  taskstats Show command console task details");
	Serial.println("  memory   Show ESP32 heap memory information");
	Serial.println("  heap     Alias for memory");
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
	const UBaseType_t taskCount = uxTaskGetNumberOfTasks();
	const TickType_t tickCount = xTaskGetTickCount();
	const TickType_t msSinceBoot = pdTICKS_TO_MS(tickCount);

	Serial.println("FreeRTOS task snapshot:");
	Serial.printf("  Number of tasks:   %u\n", static_cast<unsigned int>(taskCount));
	Serial.printf("  Uptime (ms):       %lu\n", static_cast<unsigned long>(msSinceBoot));
	Serial.printf("  Current task:      %s\n", pcTaskGetName(nullptr));
	Serial.printf("  Current priority:  %u\n", static_cast<unsigned int>(uxTaskPriorityGet(nullptr)));
	Serial.printf("  Current stack HWM: %u words\n",
		static_cast<unsigned int>(uxTaskGetStackHighWaterMark(nullptr)));
}

char *getTaskState(eTaskState state)
{
	char *name = NULL;
	static const char *s[] = {"RUN", "READY", "BLOCK", "SUSP", "DEL", "INV"};

	name = (char *)(s[state]);

	return name;
}
void printTasksInfo()
{
	UBaseType_t taskCount = uxTaskGetNumberOfTasks();

	Serial.printf("--- %u tasks executing ---\n", static_cast<unsigned int>(taskCount));
	Serial.println("Name                 State  Prio  Stack");
	Serial.println("----------------------------------------");

#if defined(configENABLE_TASK_SNAPSHOT) && (configENABLE_TASK_SNAPSHOT == 1)
	TaskSnapshot_t snapshots[kMaxTasks];
	UBaseType_t tcbSize = 0;
	UBaseType_t listed = uxTaskGetSnapshotAll(snapshots, kMaxTasks, &tcbSize);
	(void)tcbSize;

	for (UBaseType_t index = 0; index < listed; ++index)
	{
		TaskHandle_t handle = static_cast<TaskHandle_t>(snapshots[index].pxTCB);
		eTaskState state = eTaskGetState(handle);
		UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(handle);

		Serial.printf("%-20s %-6s %4u %6u\n",
			pcTaskGetName(handle),
			getTaskState(state),
			static_cast<unsigned int>(uxTaskPriorityGet(handle)),
			static_cast<unsigned int>(highWaterMark));
	}

	if (listed < taskCount)
	{
		Serial.printf("(truncated: %u task(s) not shown; increase kMaxTasks)\n",
			static_cast<unsigned int>(taskCount - listed));
	}
#else
	TaskHandle_t handle = xTaskGetCurrentTaskHandle();
	eTaskState state = eTaskGetState(handle);
	UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(handle);

	Serial.printf("%-20s %-6s %4u %6u\n",
		pcTaskGetName(handle),
		getTaskState(state),
		static_cast<unsigned int>(uxTaskPriorityGet(handle)),
		static_cast<unsigned int>(highWaterMark));
	Serial.println("(full task listing unavailable in this FreeRTOS build)");
#endif
}




void printCPUInfo(void)
{
	uint32_t cpuFreq = getCpuFrequencyMhz();
	uint8_t nCores = ESP.getChipCores();
	BaseType_t coreId = xPortGetCoreID();
	uint64_t usSinceBoot = esp_timer_get_time();

	uint32_t freeheap = ESP.getFreeHeap();
	uint32_t freesketchspace = ESP.getFreeSketchSpace();
	uint32_t sketchsize = ESP.getSketchSize();
	uint8_t revision = ESP.getChipRevision();

	const char *model = ESP.getChipModel();

	Serial.printf("------------------------------------------\n");
	Serial.printf("> chip model:         %s\n", model);
	Serial.printf("> chip revision:      %d\n", revision);
	Serial.printf("> number of cores:    %d\n", nCores);
	Serial.printf("> CPU frequency:      %lu MHz\n", cpuFreq);
	Serial.printf("> ESP32 core id:      %d\n", coreId);
	Serial.printf("> time since boot:    %llu us\n", usSinceBoot);
	Serial.printf("> free heap:          %lu bytes\n", freeheap);
	Serial.printf("> sketch size:        %lu\n", sketchsize);
	Serial.printf("> free sketch space:  %lu\n", freesketchspace);
	Serial.printf("------------------------------------------\n");
}

void printVersion(void)
{
	const char *version = ESP.getSdkVersion();

	Serial.printf("------------------------------------------\n");
	Serial.printf("> Build:     %s\n", __TIMESTAMP__);
	Serial.printf("> ESP32 SDK: %s\n", version);
	Serial.printf("> FreeRTOS:  %s\n", tskKERNEL_VERSION_NUMBER);
	Serial.printf("------------------------------------------\n");
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