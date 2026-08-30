#ifndef COMMAND_CONSOLE_H_
#define COMMAND_CONSOLE_H_

#ifdef __cplusplus
extern "C" {
#endif

void StartCommandConsoleTask(void *pvParameters);
void CommandConsoleTask(void *pvParameters);
void PrintfTask(void *pvParameters);

#ifdef __cplusplus
}
#endif

namespace command_console
{
    using CommandHandler = void (*)(const char *args);

    bool RegisterCommand(const char *name, CommandHandler handler, const char *helpText = nullptr);
    void UnregisterCommand(const char *name);
    void PrintRegisteredCommands();
    void ProcessCommandLine(const char *commandLine);
}

#endif /* COMMAND_CONSOLE_H_ */