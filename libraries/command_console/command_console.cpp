/*
 * command_console.cpp
 *
 * Revision: V2.0
 * Modified: 07-09-2026
 */ 

#include <Arduino.h>
#include <cstring>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//#include <esp_additions/freertos/task_snapshot.h>

#include "command_console.h"
#include "ts_printf.h"
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

CommandEntry commands[kMaxCommands];
size_t commandCount = 0;

// prints the "> " input prompt
void PrintPrompt()
{
	ts_printf("> ");
}

// prints the header line above the list of registered commands
void PrintBuiltInHelp()
{
	ts_printf("Available commands:\n");
	command_console::PrintRegisteredCommands();
}

// handler for the built-in "help" command
void HandleHelp(const char *args)
{
	(void)args;
	PrintBuiltInHelp();
	command_console::PrintRegisteredCommands();
}

// handler for the built-in "echo" command
void HandleEcho(const char *args)
{
	ts_printf("%s", args);
	ts_printf("\n");
}

// registers the built-in help/echo commands, only once
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

// splits a raw input line into a command name and its remaining argument text
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
// adds a command, or replaces the handler/help text if the name is already registered
bool RegisterCommand(const char *name, CommandHandler handler, const char *helpText)
{
	if (name == nullptr || handler == nullptr)
	{
		return false;
	}

	for (size_t index = 0; index < commandCount; ++index)
	{
		if (strcmp(commands[index].name, name) == 0)
		{
			commands[index].handler = handler;
			commands[index].helpText = helpText != nullptr ? helpText : commands[index].helpText;
			return true;
		}
	}

	if (commandCount >= kMaxCommands)
	{
		return false;
	}

	commands[commandCount].name = name;
	commands[commandCount].handler = handler;
	commands[commandCount].helpText = helpText;
	++commandCount;
	return true;
}

// removes a previously registered command by name
void UnregisterCommand(const char *name)
{
	if (name == nullptr)
	{
		return;
	}

	for (size_t index = 0; index < commandCount; ++index)
	{
		if (strcmp(commands[index].name, name) == 0)
		{
			for (size_t move = index + 1; move < commandCount; ++move)
			{
				commands[move - 1] = commands[move];
			}
			--commandCount;
			return;
		}
	}
}

// prints all registered commands with their help text
void PrintRegisteredCommands()
{
	if (commandCount == 0)
	{
		ts_printf("  (no custom commands registered)\n");
		return;
	}

	ts_printf("Registered commands:\n");
	for (size_t index = 0; index < commandCount; ++index)
	{
		if (commands[index].helpText != nullptr)
		{
			ts_printf("  %-10s %s\n", commands[index].name, commands[index].helpText);
		}
		else
		{
			ts_printf("  %s\n", commands[index].name);
		}
	}
}

// parses commandLine and dispatches it to the matching registered handler
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

	for (size_t index = 0; index < commandCount; ++index)
	{
		if (strcmp(commands[index].name, commandName) == 0)
		{
			commands[index].handler(args);
			return;
		}
	}

	ts_printf("Unknown command: %s\n", commandName);
	ts_printf("Type 'help' for a list of commands.\n");
}
}

// thin wrapper so file-scope code can call the namespaced PrintPrompt
void printPrompt()
{
	PrintPrompt();
}

// thin wrapper so file-scope code can call the namespaced help/command list printers
void printCommandList()
{
	PrintBuiltInHelp();
	command_console::PrintRegisteredCommands();
}

// thin wrapper so file-scope code can call the namespaced command processor
void processCommand(const char *command)
{
	command_console::ProcessCommandLine(command);
}


// creates the CommandConsoleTask FreeRTOS task
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
		ts_printf("Unable to start command console task.");
	}
}

// unused placeholder task, kept only to satisfy legacy extern "C" linkage
extern "C" void PrintfTask(void *pvParameters)
{
	(void)pvParameters;
	vTaskDelete(nullptr);
}

// reads Serial input line by line and processes each line as a console command
extern "C" void CommandConsoleTask(void *pvParameters)
{
	(void)pvParameters;
	static char input[kMaxInputSize];
	static char lastInput[kMaxInputSize];
	size_t inputIndex = 0;
	bool ignoreLineFeed = false;

	//ts_printf();
	ts_printf("FreeRTOS command server");
	printCommandList();
	//ts_printf();
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
//			ts_printf();
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
//			ts_printf();
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
				ts_printf("\b \b");
			}
		}
		else if (inputIndex < kMaxInputSize - 1)
		{
			input[inputIndex++] = character;
			input[inputIndex] = '\0';
			ts_printf("%c", character);
			//Serial.write(static_cast<uint8_t>(character));
		}
	}
}