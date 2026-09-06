# Opdracht 5: Interrupts in FreeRTOS

In dit practicum komt het gebruik van interrupts in FreeRTOS aan bod. Er zijn drie opdrachten:

- In opdracht 1 wordt het principe van interrupts en de bijbehorende scheduling uitgelegd aan de hand van het indrukken van een drukknop.
- In opdracht 2 worden periodiek interrupts gegenereerd door een clock generator.
- In opdracht 3 worden periodieke interrupts gebruikt voor het uitvoeren van een lege control task. Daarnaast wordt een afzonderlijke task gebruikt voor het instellen van een parameter voor de control task.

## 1. Hardware-interrupts met drukknoppen

Hardware-interrupts onderbreken de lopende programma-executie omdat er een externe gebeurtenis plaatsvindt, dus een gebeurtenis in de fysieke buitenwereld die onmiddellijk aandacht van de processor nodig heeft. Deze interrupts kunnen bijvoorbeeld afkomstig zijn van een extern device dat data beschikbaar heeft voor verwerking.

Door een interrupt kan de CPU op onvoorspelbare momenten worden onderbroken tijdens het uitvoeren van tasks. De code die direct en onmiddellijk moet worden uitgevoerd bij het afhandelen van een interrupt, heet een Interrupt Service Routine (ISR).

In FreeRTOS heeft een ISR een hogere prioriteit dan elke andere task. Dat betekent het volgende:

- Een ISR is **geen** FreeRTOS-task en kan niet worden geblokkeerd. In een ISR kan dus bijvoorbeeld niet op een queue, mutex of semafoor worden gewacht.
- Een ISR mag zichzelf niet blokkeren en mag dus geen potentieel oneindige herhalingen bevatten. Een ISR heeft een hogere prioriteit dan de scheduler en kan bij een oneindige herhaling ervoor zorgen dat het systeem vastloopt.
- Een ISR moet zo kort mogelijk zijn, zowel in tijd als in uitvoerbare code. In een ISR mogen alleen de hoogst noodzakelijke acties worden uitgevoerd. Welke acties dat zijn, hangt meestal af van het device dat de interrupt genereert.
- In een ISR mag slechts gebruik worden gemaakt van enkele speciale FreeRTOS-functies. Deze hebben de vorm `<functienaam>FromISR`. Andere FreeRTOS-functies mogen absoluut niet worden gebruikt, omdat het systeem anders kan crashen of vastlopen. Een voorbeeld van een FreeRTOS-functie die in een ISR wel mag worden gebruikt, is `xSemaphoreGiveFromISR`.

### Deferred Interrupt Processing

Om een interrupt toch in een normale FreeRTOS-task te kunnen afhandelen, wordt gebruikgemaakt van *Deferred Interrupt Processing* (uitgestelde interruptafhandeling). Hierbij wordt de niet-tijdkritische verwerking van de data niet in de ISR zelf uitgevoerd, maar uitgesteld en verder afgehandeld in een normale FreeRTOS-task. Meestal heeft die task een hogere prioriteit dan alle andere tasks, maar per definitie een lagere prioriteit dan de interrupt.

Een ISR heeft daarom uitsluitend de volgende beperkte functionaliteit:

- Het afhandelen van de absoluut noodzakelijke acties, zoals het registreren van de interruptbron of -oorzaak en het wegnemen van de oorzaak van de interrupt. Dit hangt vaak af van het type device dat de interrupt genereert.
- Het signaleren aan de FreeRTOS-scheduler dat een interrupt is opgetreden en dat een FreeRTOS-task de data moet verwerken. Met andere woorden: de ISR zal meestal een task deblokkeren. Dit kan bijvoorbeeld met de functie `xSemaphoreGiveFromISR`.

Zie figuur 1 voor een voorbeeld van *Deferred Interrupt Processing*. Bron: [FreeRTOS: Deferred Interrupt Processing](https://www.freertos.org/deferred_interrupt_processing.html).

**Figuur 1.** Deferred Interrupt Processing

Als de FreeRTOS-task die de interrupt moet afhandelen een voldoende hoge prioriteit heeft ten opzichte van de overige tasks, wordt deze onmiddellijk uitgevoerd nadat de zeer korte ISR klaar is. Feitelijk wordt de verwerking van de interrupt aaneengesloten in de tijd uitgevoerd, alsof alle verwerking in de ISR zelf had plaatsgevonden. Zie figuur 1: alle interruptverwerking vindt plaats tussen tijdstippen $t_2$ en $t_4$, ook al wordt een deel van de verwerking uitgevoerd door een task, het blauwe deel.

### Opdracht

Om inzicht te krijgen in de werking van interrupts, wordt in de volgende opdracht een interrupt gegenereerd door het indrukken van één of meer van de vier drukknoppen op het shield. Deze interrupt wordt afgehandeld door een ISR en een bijbehorende FreeRTOS-task. Als een knop wordt ingedrukt, gebeurt het volgende:

- De globale variabele `g_InterruptCount` wordt in de ISR met één verhoogd.
- Een FreeRTOS-task laat vervolgens de waarde van deze variabele zien, dus 1, 2, 3 enzovoort, en wordt geblokkeerd totdat er weer een knop wordt ingedrukt.

Maak in de volgende opdracht gebruik van de solution `RTSW_week_5_ButtonInterrupts_Framework.sln`. Voeg uitsluitend code toe aan, of wijzig code in, het bestand `main.c`. Alle overige folders bevatten bestanden voor de systeemconfiguratie. Laat deze **ongewijzigd**.

Zoek in het bestand `main.c` naar de tekenreeks `TODO` voor de bijbehorende aanwijzingen in de broncode.

## 2. Periodieke clock-interrupts

Mechatronische regelsystemen maken vaak gebruik van een hard periodiek kloksignaal met een vaste frequentie, afkomstig van een externe bron. De frequentie waarmee sensoren moeten worden gesampled en de frequentie waarmee actuatoren moeten worden aangestuurd, bepalen de gewenste frequentie van dat externe kloksignaal.

In dit practicum wordt gebruikgemaakt van een externe klok van 1 kHz, met een periodetijd van 1 ms. De frequentie van 1 kHz is een realistische waarde die ook bij projecten in periode 3.3 en periode 3.4 wordt toegepast.

Dit kloksignaal is afkomstig van een klokgenerator die met een vaste frequentie interrupts genereert die door FreeRTOS worden afgehandeld. De klok is afgeleid van een kristaloscillator.

De klokgenerator wordt met de daarvoor bedoelde kabel aangesloten op connector `DIG IN` (`J4`) van het RTSW-shield. Bit 0 van deze digitale input is de ingang voor het kloksignaal.

> **Let op:** deze opdracht is vergelijkbaar met de vorige opdracht, waarbij een drukknop een interrupt genereerde. Ook de code is vergelijkbaar. Een belangrijk verschil is echter dat de frequentie van de interrupts hoger is ($f = 1\ \mathrm{kHz}$, $T = 1\ \mathrm{ms}$), waardoor de interrupt-handler daarop moet worden aangepast.

### Opdracht

Maak in de volgende opdracht gebruik van de solution `RTSW_week_5_TimerInterrupts_Framework.sln`. Voeg uitsluitend code toe aan, of wijzig code in, het bestand `main.c`. Alle overige folders bevatten bestanden voor de systeemconfiguratie. Laat deze **ongewijzigd**.

Zoek in het bestand `main.c` naar de tekenreeks `TODO` voor de bijbehorende aanwijzingen in de broncode.

## 3. Periodieke clock-interrupts combineren met queues

In een mechatronisch regelsysteem is er minimaal één task die periodiek het regelalgoritme uitvoert: de control task. De periodetijd wordt bepaald door de klokfrequentie van een externe klok, zoals in opdracht 2 met een frequentie van 1 kHz. In opdracht 2 is de control task onder andere geïmplementeerd in de functie `TaskTimerInterruptHandler`.

Daarnaast zijn er meestal één of meer tasks met een andere functionaliteit, zoals het tijdens runtime instellen van een of meer parameters in de control task. In deze opdracht krijgt de control task een waarde aangeleverd door een task die één van de potmeters uitleest en de waarde daarvan in een queue zet. Deze queue bevat precies één waarde. De control task leest de waarde uit die in de queue is gezet.

### Opdracht

Maak in de volgende opdracht gebruik van de solution `RTSW_week_5_TimerInterrupts_2_Framework.sln`. Voeg uitsluitend code toe aan, of wijzig code in, het bestand `main.c`. Alle overige folders bevatten bestanden voor de systeemconfiguratie. Laat deze **ongewijzigd**.

Zoek in het bestand `main.c` naar de tekenreeks `TODO` voor de bijbehorende aanwijzingen in de broncode.

### De functie `fmap`

In deze opdracht wordt gebruikgemaakt van de functie `fmap`. Deze functie beeldt, ofwel “mapt”, de aangeboden ingangswaarde `x1` in het bereik `[in_min, in_max]` lineair af op de variabele `y1` in het uitgangsbereik `[out_min, out_max]`.

**Figuur 2.** De functie `fmap`

Voorbeeld 1: een ADC-waarde `adcValue` tussen 0 en 4095 kan worden afgebeeld op een bereik tussen -3,0 en 8,0:

```c
mappedValue = fmap(adcValue, 0, 4095, -3.0, 8.0);
```

Voorbeeld 2: de functie kan worden gebruikt om de ADC-waarde `adcValue` om te zetten naar een relevante waarde voor de gebruiker of applicatie, bijvoorbeeld een bandbreedte tussen 35 Hz en 70 Hz:

```c
bandBreedte = fmap(adcValue, 0, 4095, 35.0, 70.0);
```