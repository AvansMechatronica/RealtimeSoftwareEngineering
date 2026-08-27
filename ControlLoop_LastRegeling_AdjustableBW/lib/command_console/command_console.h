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

#endif /* COMMAND_CONSOLE_H_ */