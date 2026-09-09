# Systeeminformatie-bibliotheek

## Overzicht

`system_info` bevat diverse runtime-informatie-helpers voor het weergeven van systeemstatus, taakinfo en geheugeninzicht op een ESP32-platform.

## API

```cpp
void RegisterSystemInfoCommands();
void PrintMemoryInfo();
void PrintTaskStats();
void PrintTasksInfo();
void PrintCPUInfo(void);
void PrintVersion(void);
```

## Gebruik

```cpp
RegisterSystemInfoCommands();
PrintMemoryInfo();
PrintTaskStats();
```

## Doel

Deze helpers zijn ideaal voor monitoring, debugging en statusdiagnostiek tijdens ontwikkeling of in operationele firmware.

## Opmerkingen

- Ze zijn ontworpen voor gebruik in een FreeRTOS-omgeving.
- De functies zijn bedoeld om informatie te printen naar een console of debug-output.
