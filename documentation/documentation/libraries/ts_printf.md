# TS Printf Library

## Overzicht

`ts_printf` is een kleine debug- en trace-printf-helper voor embedded systemen. Het biedt een task-safe print-facade en een `ts_debug` macro die alleen in debug-builds actief is.

## API

```cpp
#ifdef DEBUG
#define ts_debug(...) ts_printf(__VA_ARGS__)
#else
#define ts_debug(...) do { } while (0)
#endif

void StartTsPrintfTask(void *pvParameters);
void ts_printf(const char *format, ...);
```

## Gebruik

```cpp
#include "ts_printf.h"

ts_debug("Temperatuur: %d\n", 42);
```

## Doel

Deze bibliotheek is bedoeld voor logs, debug-output en diagnostische berichten zonder steeds expliciete runtime-checks in de code te hoeven plaatsen.

## Opmerkingen

- `ts_debug` is automatisch uitgeschakeld buiten een `DEBUG` build.
- De task-afhandeling maakt het geschikt voor FreeRTOS-gebaseerde firmware.
