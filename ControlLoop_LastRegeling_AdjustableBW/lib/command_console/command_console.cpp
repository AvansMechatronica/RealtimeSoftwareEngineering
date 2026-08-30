#include <Arduino.h>
#include <cstring>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//#include <esp_additions/freertos/task_snapshot.h>

#include "command_console.h"
#include "vprintf.h"
#include "system_info.h"

constexpr size_t kMaxInputSize = 1000;
constexpr size_t kMaxCommands = 32;

namespace
{
struct CommandEntry
{
	const char *name;
	command_console::CommandHandler handler;
	const char *helpText;
};

CommandEntry g_commands[kMaxCommands];
size_t g_commandCount = 0;

void PrintPrompt()
{
	vPrint("> ");
}

void PrintBuiltInHelp()
{
	vPrint("Available commands:\n");
	command_console::PrintRegisteredCommands();
}

void HandleHelp(const char *args)
{
	(void)args;
	PrintBuiltInHelp();
	command_console::PrintRegisteredCommands();
}

void HandleEcho(const char *args)
{
	vPrint("%s", args);
	vPrint("\n");
}

void RegisterDefaultCommands()
{
	static bool initialized = false;
	if (initialized)
	{
		return;
	}
	initialized = true;

	command_console::RegisterCommand("help", &HandleHelp, "Show this help");

	command_console::RegisterCommand("echo", &HandleEcho, "Echo text, for example: echo hello");
}

bool ParseCommandLine(const char *commandLine, char *commandName, size_t commandNameSize, const char **args)
{
	if (commandLine == nullptr || commandName == nullptr || args == nullptr)
	{	return false;
	}

	while (*commandLine == ' ' || *commandLine == '\t')
	{
		++commandLine;
	}

	if (*commandLine == '\0')
	{
		return false;
	}

	size_t index = 0;
	while (*commandLine != '\0' && *commandLine != ' ' && *commandLine != '\t' && *commandLine != '\r' && *commandLine != '\n' && index + 1 < commandNameSize)
	{
		commandName[index++] = *commandLine++;
	}
	commandName[index] = '\0';

	while (*commandLine == ' ' || *commandLine == '\t')
	{
		++commandLine;
	}

	*args = commandLine;
	return true;
}
}

namespace command_console
{
bool RegisterCommand(const char *name, CommandHandler handler, const char *helpText)
{
	if (name == nullptr || handler == nullptr)
	{
		return false;
	}

	for (size_t index = 0; index < g_commandCount; ++index)
	{
		if (strcmp(g_commands[index].name, name) == 0)
		{
			g_commands[index].handler = handler;
			g_commands[index].helpText = helpText != nullptr ? helpText : g_commands[index].helpText;
			return true;
		}
	}

	if (g_commandCount >= kMaxCommands)
	{
		return false;
	}

	g_commands[g_commandCount].name = name;
	g_commands[g_commandCount].handler = handler;
	g_commands[g_commandCount].helpText = helpText;
	++g_commandCount;
	return true;
}

void UnregisterCommand(const char *name)
{
	if (name == nullptr)
	{
		return;
	}

	for (size_t index = 0; index < g_commandCount; ++index)
	{
		if (strcmp(g_commands[index].name, name) == 0)
		{
			for (size_t move = index + 1; move < g_commandCount; ++move)
			{
				g_commands[move - 1] = g_commands[move];
			}
			--g_commandCount;
			return;
		}
	}
}

void PrintRegisteredCommands()
{
	if (g_commandCount == 0)
	{
		vPrint("  (no custom commands registered)\n");
		return;
	}

	vPrint("Registered commands:\n");
	for (size_t index = 0; index < g_commandCount; ++index)
	{
		if (g_commands[index].helpText != nullptr)
		{
			vPrint("  %-10s %s\n", g_commands[index].name, g_commands[index].helpText);
		}
		else
		{
			vPrint("  %s\n", g_commands[index].name);
		}
	}
}

void ProcessCommandLine(const char *commandLine)
{
	if (commandLine == nullptr)
	{
		return;
	}

	char commandName[64];
	const char *args = nullptr;
	if (!ParseCommandLine(commandLine, commandName, sizeof(commandName), &args))
	{
		return;
	}

	for (size_t index = 0; index < g_commandCount; ++index)
	{
		if (strcmp(g_commands[index].name, commandName) == 0)
		{
			g_commands[index].handler(args);
			return;
		}
	}

	vPrint("Unknown command: %s\n", commandName);
	vPrint("Type 'help' for a list of commands.\n");
}
}

void printPrompt()
{
	PrintPrompt();
}

void printCommandList()
{
	PrintBuiltInHelp();
	command_console::PrintRegisteredCommands();
}

void processCommand(const char *command)
{
	command_console::ProcessCommandLine(command);
}


extern "C" void StartCommandConsoleTask(void *pvParameters)
{
	(void)pvParameters;

	RegisterDefaultCommands();

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