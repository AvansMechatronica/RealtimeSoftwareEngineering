# FreeRTOS-issues in dit project

Dit document beschrijft concrete FreeRTOS-gerelateerde risico's en aandachtspunten die zijn
aangetroffen in de huidige codebase (`src/`). Per punt staat aangegeven waar het probleem zich
bevindt, waarom het een risico is, en welke oplossingsrichting wordt aanbevolen.

## 1. Alle applicatietaken draaien op prioriteit 0 (idle-niveau)

**Locatie:** [src/application_tasks.cpp](src/application_tasks.cpp#L86-L96)

```cpp
xTaskCreate(ControlTask, "tsk_Control", 4 * configMINIMAL_STACK_SIZE, hardwareConfig, 0, &controlTaskHandle);
xTaskCreate(ButtonHandlerTask, "tsk_Button", 4 * configMINIMAL_STACK_SIZE, hardwareConfig, 0, &buttonHandlerTaskHandle);
xTaskCreate(ParameterSettingTask, "tsk_ParamHandler", 4 * configMINIMAL_STACK_SIZE, hardwareConfig, 0, &parameterSettingTaskHandle);
```

Alle drie de taken krijgen prioriteit `0`, wat in FreeRTOS gelijk is aan `tskIDLE_PRIORITY`. Dit
betekent dat:

- `ControlTask` (de tijdkritische 1 ms regellus) **geen enkele prioriteit heeft boven** de
  knoppen-/parametertaken of zelfs de idle-taak.
- Er is geen garantie dat de regellus op tijd wordt uitgevoerd wanneer een andere taak
  van "gelijke" prioriteit actief blijft (round-robin scheduling).
- Op ESP32 voedt de idle-taak per core de Task Watchdog Timer (TWDT). Als taken op
  hetzelfde prioriteitsniveau de idle-taak "verhongeren" (zie punt 2), kan dit leiden tot
  een watchdog-reset.

**Advies:** geef `ControlTask` de hoogste prioriteit (bv. `configMAX_PRIORITIES - 1` of een vast
hoog niveau), en geef `ButtonHandlerTask`/`ParameterSettingTask` een lagere, maar wel
boven-idle prioriteit (bv. 1 en 2).

## 2. Busy-wait polling-lussen zonder `vTaskDelay`

**Locaties:**
- [src/button_handler_task.cpp](src/button_handler_task.cpp#L48-L52)
- [src/parameter_setting_task.cpp](src/parameter_setting_task.cpp#L64-L68)

```cpp
// wait until button released:
while (restartButton.IsPressed(restartButtonIndex))
{
}
```

```cpp
// wait until button released:
while (hardwareConfig->buttons.IsPressed(buttonNumber))
{
}
```

Dit zijn **busy-wait lussen zonder `vTaskDelay()` of andere blokkerende call**. Zolang de knop
ingedrukt blijft, claimt de taak 100% CPU-tijd op de core waarop hij draait, zonder ooit terug te
geven aan de scheduler. In combinatie met punt 1 (prioriteit gelijk aan idle) kan dit:

- de idle-taak volledig verhongeren → Task Watchdog Timer reset op ESP32.
- andere taken met dezelfde of lagere prioriteit (bv. `ParameterSettingTask` tijdens het
  ingedrukt houden van de knop in `ButtonHandlerTask`) laten stilvallen.

**Advies:** voeg een korte `vTaskDelay(pdMS_TO_TICKS(10))` toe in deze polling-lussen, net zoals
elders in dezelfde taken al gebeurt.

## 3. Stack-overflow- en malloc-failure-hooks staan uit

**Locatie:** [src/main.cpp](src/main.cpp#L44-L72)

```cpp
#if 0
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName)
{ ... }

void vApplicationMallocFailedHook(void)
{ ... }
#endif
```

Beide hooks zijn met `#if 0` uitgeschakeld. Dit betekent dat een stack-overflow van een taak (of
een mislukte heap-allocatie) **stil** kan leiden tot corruptie of een onvoorspelbare crash, in
plaats van een duidelijke foutmelding. Dit is met name risicovol omdat de stackgroottes van de
taken (zie punt 4) niet zijn gevalideerd.

**Advies:** schakel beide hooks in (`#if 1` of verwijder de guard) en zorg dat
`configCHECK_FOR_STACK_OVERFLOW` in de FreeRTOS-configuratie op 2 staat.

## 4. Stackgroottes zijn "geraden", niet gemeten

**Locatie:** [src/application_tasks.cpp](src/application_tasks.cpp#L86-L96)

Alle taken krijgen `4 * configMINIMAL_STACK_SIZE` als stackgrootte, zonder dat dit is
onderbouwd met `uxTaskGetStackHighWaterMark()`-metingen. In combinatie met de uitgeschakelde
stack-overflow hook (punt 3) is er geen enkel vangnet als dit te krap blijkt te zijn.

**Advies:** meet de daadwerkelijke stackgebruik per taak tijdens bedrijf en stem de
stackgroottes daarop af, en/of log periodiek de high-water-mark.

## 5. Dubbele/ongebruikte `HardwareConfig`-instantie in `control_task.cpp`

**Locatie:** [src/control_task.cpp](src/control_task.cpp#L53)

```cpp
static HardwareConfig hardwareConfig;   // bestandsscope, wordt nooit gebruikt/geïnitialiseerd
...
void ControlTask(void *pvParameters)
{
    HardwareConfig *hardwareConfig = (HardwareConfig *)pvParameters;  // schaduwt de static hierboven
```

Er bestaat een ongebruikte statische `HardwareConfig` op bestandsniveau die nooit via
`ConfigureHardware()` wordt geïnitialiseerd, en die door de lokale parameter met dezelfde naam
wordt overschaduwd. Dit is verwarrend en foutgevoelig: een toekomstige wijziging die per ongeluk
de verkeerde variabele gebruikt, benadert ongeïnitialiseerd hardware-state.

**Advies:** verwijder de ongebruikte statische variabele.

## 6. Geen affiniteit met een specifieke core voor de tijdkritische regellus

**Locatie:** [src/application_tasks.cpp](src/application_tasks.cpp#L86)

`ControlTask` wordt aangemaakt met `xTaskCreate()` (niet `xTaskCreatePinnedToCore()`), waardoor
de FreeRTOS-scheduler op de (dual-core) ESP32 vrij is om de taak tussen de cores te laten
migreren. Voor een 1 ms-periodieke regellus kan dit onnodige jitter introduceren, zeker in
combinatie met Wi-Fi/Bluetooth-taken die standaard op core 0 draaien.

**Advies:** pin `ControlTask` expliciet aan een core met `xTaskCreatePinnedToCore()`, bijvoorbeeld
core 1, en houd niet-tijdkritische taken op core 0.

## 7. Foutafhandeling bij creatie van RTOS-objecten is leeg

**Locatie:** [src/application_tasks.cpp](src/application_tasks.cpp#L58-L96)

```cpp
handle_ParameterQueue = xQueueCreate(parameterQueueSize, sizeof(double));
if (handle_ParameterQueue == NULL)
{
}
```

Meerdere `if (... == NULL) { }`-blokken zijn leeg: als het aanmaken van de queue, semafoor,
event group, of een taak mislukt (bv. door onvoldoende heap), gaat het programma **stilzwijgend**
door alsof er niets aan de hand is. Dit kan later tot een moeilijk te herleiden null-pointer
dereference leiden (bv. `xQueuePeek(handle_ParameterQueue, ...)` in `ControlTask` met
`handle_ParameterQueue == NULL`).

**Advies:** log de fout (zoals al gebeurt bij `ConfigureHardware()`) en stop het systeem in een
veilige toestand (`while(1);` of reset), consistent met het patroon dat al voor
`ConfigureHardware()` wordt gebruikt.

## 8. Race-risico op de gedeelde parameter-queue bij opstart

**Locatie:** [src/control_task.cpp](src/control_task.cpp#L145-L182) en
[src/parameter_setting_task.cpp](src/parameter_setting_task.cpp#L69-L74)

`ControlTask` wacht met een event group (`BIT_0 | BIT_1`) tot beide hulptaken zijn opgestart
voordat de queue wordt gelezen. Dit patroon is op zich correct, maar is **impliciet**: er is geen
commentaar/assert die de aanname vastlegt dat `ParameterSettingTask` de queue altijd vult vóór
`xEventGroupSetBits(..., BIT_1)`. Een toekomstige refactor kan deze volgorde per ongeluk omdraaien
zonder dat dit opvalt in code review.

**Advies:** voeg een korte comment of `configASSERT` toe die deze volgorde-afhankelijkheid
expliciet vastlegt.

## Samenvatting prioriteiten

| # | Issue | Ernst |
|---|-------|-------|
| 1 | Alle taken op idle-prioriteit (0) | Hoog |
| 2 | Busy-wait zonder `vTaskDelay` in knopafhandeling | Hoog |
| 3 | Stack-overflow/malloc-hooks uitgeschakeld | Hoog |
| 4 | Ongevalideerde stackgroottes | Middel |
| 5 | Ongebruikte/schaduwende `HardwareConfig`-variabele | Laag |
| 6 | Geen core-affiniteit voor `ControlTask` | Middel |
| 7 | Lege foutafhandeling bij RTOS-objectcreatie | Middel |
| 8 | Impliciete volgorde-aanname parameter-queue | Laag |
