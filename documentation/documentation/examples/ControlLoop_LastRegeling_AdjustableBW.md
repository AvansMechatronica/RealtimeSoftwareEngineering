# ControlLoop_LastRegeling_AdjustableBW

## Overzicht

Dit project is een real-time regelapplicatie voor een ESP32 (FreeRTOS/Arduino-framework, via
PlatformIO) die de positie regelt van een slede die via een tandriem wordt aangedreven door een
motor (motorzijde) en waarbij een last aan de andere kant van de tandriem hangt (lastzijde). De
regelaar heeft een **instelbare bandbreedte** (adjustable bandwidth, `wblFactor`), die tijdens
bedrijf via een potentiometer/ADC-kanaal live kan worden bijgesteld.

## Functionaliteit

- **Positieregeling (`position_controller_motor.cpp`)**: implementeert het mechanische model
  (massa's, traagheidsmomenten, overbrengingsverhoudingen, torsiestijfheid van de tandriem) en de
  regellus die elke 1 ms (`Ts = 0.001 s`) wordt uitgevoerd. De regelbandbreedte wordt bepaald door
  de parameter `wblFactor`.
- **Motoraansturing (`motor_control.cpp`)**: stuurt de ESCON-motorcontroller aan via DAC-uitgangen,
  leest digitale ingangen (eindschakelaars links/rechts, overload-detectie, ATOM-foutstatus) en
  biedt functies om de slede naar de home-positie te bewegen (`MotorGotoHomePosition`) of te
  stoppen.
- **Regellus-taak (`control_task.cpp`)**: draait op een hardware-timer van 1 ms
  (`InitializePeriodicTimer`) die via een ISR een semafoor geeft om de regellus te ontgrendelen.
  Bij het opstarten wordt gewacht op de knoppen- en parametertaak (via een event group), waarna de
  slede naar de linker home-positie beweegt en de quadratuur-encoders worden gereset. Met de
  restart-knop (SW1) wordt de regellus gestart/herstart.
- **Parameterinstelling (`parameter_setting_task.cpp`)**: leest continu een ADC-kanaal uit
  (potentiometer) en zet dit om naar de bandbreedtefactor `wblFactor`
  (bereik `WBLFACTOR_MIN`..`WBLFACTOR_MAX`). Bij een significante wijziging
  (> `WBLTHRESHOLD`) wordt de nieuwe waarde doorgegeven aan de regellus via een queue
  (`handle_ParameterQueue`). Met knop B2 kan de huidige waarde worden opgevraagd/getoond.
- **Knopafhandeling (`button_handler_task.cpp`)**: bewaakt de restart-knop (SW1) en geeft bij een
  druk op de knop een semafoor (`handle_RestartSemaphore`) om de regellus te (her)starten of te
  onderbreken.
- **Hardwareconfiguratie (`hardware_config.cpp`)**: initialiseert en bundelt alle
  hardware-interfaces (digitale I/O, SPI-bus, quadratuurtellers, DAC, ADC, knoppen) in één
  `HardwareConfig`-structuur die als gedeelde context aan alle taken wordt doorgegeven.

## Taken (FreeRTOS)

| Taak | Bestand | Rol |
|------|---------|-----|
| `ControlTask` | control_task.cpp | 1 ms regellus, aansturing motor/positie |
| `ButtonHandlerTask` | button_handler_task.cpp | detecteert restart-knop SW1 |
| `ParameterSettingTask` | parameter_setting_task.cpp | leest bandbreedte-potmeter uit, toont waarde op B2 |

Deze taken worden opgestart via `StartApplicationTasks()` in `application_tasks.cpp`, samen met
een event group, binaire semafoor en queue om te synchroniseren en parameters uit te wisselen.

## Overige onderdelen

- **OLED-display**: toont statusinformatie (optioneel, via `INCLUDE_OLED_DISPLAY` build-flag).
- **Command console**: registreert commando's zoals `controlloopstats` om regellus-statistieken
  (timerinterrupts, gemiste ticks, loop-teller) op te vragen.
- **Heartbeat-taak**: knippert een status-LED om aan te geven dat het systeem draait.

## Build

Gebouwd met PlatformIO voor het board `esp32doit-devkit-v1` (Arduino-framework). Zie
[platformio.ini](platformio.ini) voor build-flags en library-afhankelijkheden.


