# Heartbeat Library

## Overzicht

`heartbeat` levert een eenvoudige heartbeat-task voor een ESP32-pagina of systeemstatusindicatie. De bibliotheek gebruikt een GPIO-pin om periodiek een signaal te togglen.

## API

```cpp
void StartHeartbeatTask(unsigned int pin);
```

## Gebruik

```cpp
#include "heartbeat.h"

StartHeartbeatTask(GPIO_NUM_2);
```

## Doel

Het is bedoeld als visuele indicatie dat het systeem nog actief is, bijvoorbeeld voor debugging of run-state monitoring.

## Opmerkingen

- De implementatie is task-gebaseerd en werkt via FreeRTOS.
- De pin is aangewezen door de gebruiker bij het starten van de taak.
