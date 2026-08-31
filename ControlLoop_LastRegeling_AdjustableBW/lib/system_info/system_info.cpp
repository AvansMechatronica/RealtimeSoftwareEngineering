#include <Arduino.h>
#include <cstring>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//#include <esp_additions/freertos/task_snapshot.h>
#include "system_info.h"
#include "vprintf.h"
#include "command_console.h"
using command_console::RegisterCommand;

#define MAX_TASKS kMaxTasks
constexpr size_t kMaxInputSize = 1000;
constexpr UBaseType_t kMaxTasks = 32;


void RegisterSystemInfoCommands()
{
	RegisterCommand("taskstats", [](const char *args) {
		(void)args;
		printTaskStats();
	}, "Show command console task details");

	RegisterCommand("tasks", [](const char *args) {
		(void)args;
		printTasksInfo();
	}, "Show current task snapshot");

	RegisterCommand("memory", [](const char *args) {
		(void)args;
		printMemoryInfo();
	}, "Show ESP32 heap memory information");

	RegisterCommand("version", [](const char *args) {
		(void)args;
		printVersion();
	}, "Show firmware information");

	RegisterCommand("cpuinfo", [](const char *args) {
		(void)args;
		printCPUInfo();
	}, "Show ESP32 CPU/chip information");
}

void printMemoryInfo()
{
	vPrint("ESP32 heap memory:");
	vPrint("  Total heap:       %lu bytes\n", ESP.getHeapSize());
	vPrint("  Free heap:        %lu bytes\n", ESP.getFreeHeap());
	vPrint("  Minimum free:     %lu bytes\n", ESP.getMinFreeHeap());
	vPrint("  Largest block:    %lu bytes\n", ESP.getMaxAllocHeap());
}

void printTaskStats()
{
	const UBaseType_t taskCount = uxTaskGetNumberOfTasks();
	const TickType_t tickCount = xTaskGetTickCount();
	const TickType_t msSinceBoot = pdTICKS_TO_MS(tickCount);

	vPrint("FreeRTOS task snapshot:");
	vPrint("  Number of tasks:   %u\n", static_cast<unsigned int>(taskCount));
	vPrint("  Uptime (ms):       %lu\n", static_cast<unsigned long>(msSinceBoot));
	vPrint("  Current task:      %s\n", pcTaskGetName(nullptr));
	vPrint("  Current priority:  %u\n", static_cast<unsigned int>(uxTaskPriorityGet(nullptr)));
	vPrint("  Current stack HWM: %u words\n",
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
	const UBaseType_t taskCount = uxTaskGetNumberOfTasks();

	vPrint("--- %u tasks executing ---\n", static_cast<unsigned int>(taskCount));
	vPrint("Name                 State  Prio  Stack\n");
	vPrint("----------------------------------------\n");

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

		vPrint("%-20s %-6s %4u %6u\n",
			pcTaskGetName(handle),
			getTaskState(state),
			static_cast<unsigned int>(uxTaskPriorityGet(handle)),
			static_cast<unsigned int>(highWaterMark));
	}

	if (listed < taskCount)
	{
		vPrint("(truncated: %u task(s) not shown; increase kMaxTasks)\n",
			static_cast<unsigned int>(taskCount - listed));
	}
#elif defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1)
	TaskStatus_t tasks[kMaxTasks];
	const UBaseType_t listed = uxTaskGetSystemState(tasks, kMaxTasks, nullptr);

	for (UBaseType_t index = 0; index < listed; ++index)
	{
		const TaskStatus_t &status = tasks[index];
		const eTaskState state = status.eCurrentState;
		const UBaseType_t highWaterMark = status.usStackHighWaterMark;

		vPrint("%-20s %-6s %4u %6u\n",
			status.pcTaskName,
			getTaskState(state),
			static_cast<unsigned int>(status.uxCurrentPriority),
			static_cast<unsigned int>(highWaterMark));
	}

	if (listed < taskCount)
	{
		vPrint("(truncated: %u task(s) not shown; increase kMaxTasks)\n",
			static_cast<unsigned int>(taskCount - listed));
	}
#else
	vPrint("Task diagnostics unavailable in this build (no task snapshot/trace support).\n");
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

	vPrint("------------------------------------------\n");
	vPrint("> chip model:         %s\n", model);
	vPrint("> chip revision:      %d\n", revision);
	vPrint("> number of cores:    %d\n", nCores);
	vPrint("> CPU frequency:      %lu MHz\n", cpuFreq);
	vPrint("> ESP32 core id:      %d\n", coreId);
	vPrint("> time since boot:    %llu us\n", usSinceBoot);
	vPrint("> free heap:          %lu bytes\n", freeheap);
	vPrint("> sketch size:        %lu\n", sketchsize);
	vPrint("> free sketch space:  %lu\n", freesketchspace);
	vPrint("------------------------------------------\n");
}

void printVersion(void)
{
	const char *version = ESP.getSdkVersion();

	vPrint("------------------------------------------\n");
	vPrint("> Build:     %s\n", __TIMESTAMP__);
	vPrint("> ESP32 SDK: %s\n", version);
	vPrint("> FreeRTOS:  %s\n", tskKERNEL_VERSION_NUMBER);
	vPrint("------------------------------------------\n");
}
