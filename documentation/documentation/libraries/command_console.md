# Command Console Library

## Overzicht

`command_console` biedt een eenvoudige command-based console voor een ESP32-project. Het maakt het mogelijk om opdrachten te registreren, uit te voeren en overzichtelijk weer te geven via een tekstinterface.

## API

```cpp
namespace command_console {
    using CommandHandler = void (*)(const char *args);

    bool RegisterCommand(const char *name, CommandHandler handler, const char *helpText = nullptr);
    void UnregisterCommand(const char *name);
    void PrintRegisteredCommands();
    void ProcessCommandLine(const char *commandLine);
}
```

## Vrije functies

```cpp
void StartCommandConsoleTask(void *pvParameters);
void CommandConsoleTask(void *pvParameters);
void PrintfTask(void *pvParameters);
```

## Gebruik

```cpp
static void HelpHandler(const char *args) {
    command_console::PrintRegisteredCommands();
}

command_console::RegisterCommand("help", HelpHandler, "Toon beschikbare commando's");
command_console::ProcessCommandLine("help");
```

## Doel

Deze library is ideaal voor debugging, systeemcontrole en runtime-commando's zonder een complete terminal-stack te hoeven toevoegen.

## Opmerkingen

- De console werkt via een task-model dat compatibel is met FreeRTOS.
- Commando's kunnen dynamisch worden geregistreerd en verwijderd.
