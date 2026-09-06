# Opdracht 2: Tasks en priorities in FreeRTOS

In dit practicum komen de volgende onderwerpen aan bod:

•	Het maken van tasks (= taak, threads) in FreeRTOS
•	Het wijzigen van task priorities
•	Het gebruik van de functie vPrintString
•	Het gebruik van delays en het effect daarvan op de uitvoering van tasks


1.	Aansturen van de LED’s op het RTSW-shield

In dit practicum worden enkele van de 4 LED’s op het RTSW-shield aangestuurd. Voor het aansturen van een LED  wordt de volgende functie gebruikt:

led_SetState(uint8_t ledNumber, bool ledOn)

ledNumber is het nummer van de aan te sturen LED, en moet liggen ligt tussen 0 en 3:
•	0 = meest RECHTSE LED, bit 0, Least significant Bit (aangegeven op het board met D1)
•	3 = meest LINKSE LED, bit 3, Most Significant bit (aangegeven op het board met D4)

ledOn is een boolean met de waarden:
•	true:	LED aan
# Opdracht 2: Tasks en priorities in FreeRTOS

In dit practicum komen de volgende onderwerpen aan bod:

- Het maken van tasks (taken of threads) in FreeRTOS
- Het wijzigen van task priorities
- Het gebruik van de functie `vPrintString`
- Het gebruik van delays en het effect daarvan op de uitvoering van tasks

## 1. Aansturen van de LED's op het RTSW-shield

In dit practicum worden enkele van de vier LED's op het RTSW-shield aangestuurd. Voor het aansturen van een LED wordt de volgende functie gebruikt:

```c
led_SetState(uint8_t ledNumber, bool ledOn)
```

`ledNumber` is het nummer van de aan te sturen LED en moet tussen 0 en 3 liggen:

- `0` = meest rechtse LED, bit 0, Least Significant Bit (op het board aangegeven met D1)
- `3` = meest linkse LED, bit 3, Most Significant Bit (op het board aangegeven met D4)

`ledOn` is een boolean met de volgende waarden:

- `true`: LED aan
- `false`: LED uit

## 2. Tasks in FreeRTOS

Gebruik in de volgende opdrachten de solution `RTSW_week_2_Framework.sln`. Voeg uitsluitend code toe aan, of wijzig code in, het bestand `main.c`. Alle overige folders bevatten bestanden voor de systeemconfiguratie. Laat deze **ongewijzigd**.

In de functie `StartUserTasks()` is onder andere een task gecreëerd die een LED laat knipperen. De code die door deze task wordt uitgevoerd, staat in de functie `UserTask`. Deze task wordt gemaakt en gestart met de volgende code, die gebruikmaakt van de FreeRTOS-functie `xTaskCreate`:

```c
result = xTaskCreate(UserTask, "tsk_User", configMINIMAL_STACK_SIZE,
					 NULL, priority, &handle_UserTask);
```

De parameters voor deze functie zijn achtereenvolgens:

- `UserTask`: de functie die door de task wordt uitgevoerd. Bekijk in de broncode wat deze functie doet.
- `"tsk_User"`: de human-readable naam waaronder deze task wordt uitgevoerd. Deze naam wordt onder andere gebruikt door de commando's `task-stats` en `run-time-stats`.
- `configMINIMAL_STACK_SIZE`: de grootte van de stack (de toegewezen geheugenruimte) in bytes.
- `NULL`: de parameter die aan de task kan worden meegegeven. Voorlopig is deze `NULL` (een null-pointer, oftewel ongebruikt).
- `priority`: de prioriteit van de task. Tasks met een hogere prioriteit hebben bij het gebruik van de CPU voorrang op tasks met een lagere prioriteit. Prioriteit 0 is het laagst en prioriteit 4 het hoogst in deze practicumconfiguratie.
- `&handle_UserTask`: het adres van de handle die `xTaskCreate` aan de aanroeper teruggeeft. Met deze handle kan later, indien nodig, naar deze task worden verwezen.

### Aanwijzingen

- Declareer een variabele van het juiste type met de naam `handle_UserTask_2`.
- Maak een functie `UserTask_2` die LED D2 laat knipperen. Deze functie wordt dus bijna een kopie van `UserTask`.
- Voeg aan de functie `StartUserTasks()` de bijbehorende code toe om `UserTask_2` te starten.
- Geef de nieuwe task de naam `"tsk_User_2"`.
- Geef `UserTask_2` dezelfde prioriteit als `UserTask`; beide hebben prioriteit 0.

### Voorbeelden van uitvoer

Voorbeeld van de programma-uitvoer in Termite:

*Figuur 1. Termite terminal window*

Voorbeeld van `task-stats`:

Voorbeeld van `run-time-stats`:

## 3. Task priorities in FreeRTOS

Het resultaat van de vorige opdrachten is een programma met twee onafhankelijke tasks, waarbij elke task één LED laat knipperen. Beide tasks hebben dezelfde prioriteit, namelijk 0.

### Aanwijzingen

- Gebruik de task handle die bij `xTaskCreate` is aangemaakt.
- De prioriteit van `UserTask_2` kan op elk willekeurig moment door iedereen worden aangepast, dus ook bijvoorbeeld in `UserTask`.

## 4. Delay- en sleepfuncties

Een task hoeft niet altijd bezig te zijn met de uitvoering van een programma. Tijdens de uitvoering bevindt een task zich in de `ready`- of `running`-state. Een task kan ook wachten op een bepaalde gebeurtenis of totdat een bepaalde tijd is verstreken. In dat geval bevindt de task zich in de zogenoemde `blocked`-state.

De tasks uit de voorgaande opdrachten bevonden zich altijd in de `running`- of `ready`-state. Dat is ook het geval wanneer een task met de functie `dirtyDelay` actief staat te wachten totdat een bepaalde tijd is verstreken.

De functie `dirtyDelay` is echter zeer inefficiënt: de CPU voert hierin zinloze acties uit, zoals optellen, om tijd te doden. Het is veel efficiënter om een task in de `blocked`-state te zetten en de task te laten wachten totdat de wachttijd voorbij is. Daarna kan de task weer tijdelijk worden uitgevoerd. Gedurende die wachttijd kunnen andere tasks door de CPU worden uitgevoerd. Dit kan worden gerealiseerd door `dirtyDelay` te vervangen door een efficiëntere FreeRTOS-delayfunctie.

De functie `vTaskDelay` werkt met clock ticks van het systeem. Het is echter vaak makkelijker om tijden in (milli)seconden te specificeren. Hiervoor is de bibliotheekfunctie `taskSleep` beschikbaar. Deze functie maakt gebruik van `vTaskDelay` en rekent de gewenste tijd, gespecificeerd in milliseconden, om naar het bijbehorende aantal ticks.

Bij de volgende opdrachten wordt gebruikgemaakt van de code zoals die tot en met opdracht 9 is gemaakt. Dat betekent dat `UserTask` prioriteit 0 heeft en `UserTask_2` prioriteit 1 of 2, in elk geval hoger dan 0. Het effect hiervan is dat de task met de hoogste prioriteit (1 of 2) alle CPU-tijd krijgt, waardoor alleen de bijbehorende LED knippert. De andere LED blijft uit, omdat de bijbehorende task een lagere prioriteit heeft (0) en dus niet aan de beurt komt.


 





 



De functie vTaskDelay werkt met clock ticks van het systeem, het is echter vaak makkelijker om tijden te specificeren in (milli)seconden. Hiervoor is de bibliotheekfunctie taskSleep beschikbaar, die (uiteraard) weer gebruik maakt van vTaskDelay, en de gewenste tijd, gespecificeerd in milliseconden, omrekent naar het daarbij behorende aantal ticks.
