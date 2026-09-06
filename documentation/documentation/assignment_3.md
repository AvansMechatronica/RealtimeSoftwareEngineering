# Opdracht 3: Mutual exclusion en task synchronisation in FreeRTOS

In dit practicum komen de volgende onderwerpen aan bod:

- Het gebruik van mutexes in FreeRTOS voor toegang tot gedeelde resources
- Het synchroniseren van tasks
- Het gebruik van de ADC

## 1. Shared resources in FreeRTOS

Gegeven de volgende situatie: in een dierentuin zijn er vier entreepoortjes, waarmee het totaal aantal bezoekers in de dierentuin moet worden geteld. Er worden alleen inkomende bezoekers geteld.

- Het totale aantal inkomende bezoekers wordt opgeslagen in de globale variabele `G_NumberOfVisitors`.
- Elk entreepoortje wordt gesimuleerd door één task.
- Een inkomende bezoeker wordt geregistreerd door de globale variabele `G_NumberOfVisitors` met één te verhogen.
- Zodra een entreepoortje 100.000 bezoekers heeft geteld, laat de bijbehorende task het op dat moment totaal aantal getelde bezoekers zien, dus van alle vier de poortjes samen. Daarna gaat de task oneindig slapen. De task die als laatste klaar is, zou dus een totaal van `4 * 100000 = 400000` bezoekers moeten laten zien.

Omdat alle entreepoortjes volledig identiek functioneren, wordt in dit geval één C-functie `UserTask` gedefinieerd, waarvan vier kopieën in vier tasks worden gestart. Om deze tasks van elkaar te kunnen onderscheiden, wordt bij `xTaskCreate` een parameter meegegeven: het volgnummer van het entreepoortje, met de waarden 1 tot en met 4. `xTaskCreate` kan bijvoorbeeld als volgt worden aangeroepen:

```c
xTaskCreate(UserTask, "tsk_Counter", configMINIMAL_STACK_SIZE,
            (void *)(zooCounter),
            priority, &handle_UserTask);
```

De variabele `zooCounter` is in dit geval van het type integer en heeft achtereenvolgens de waarden 1 tot en met 4.

### Opdracht

Maak in de volgende opdracht gebruik van de solution `RTSW_week_3_Zoo_Framework.sln`. Voeg uitsluitend code toe aan, of wijzig code in, het bestand `main.c`. Alle overige folders bevatten bestanden voor de systeemconfiguratie. Laat deze **ongewijzigd**.

De globale variabele `G_NumberOfVisitors` wordt door vier tasks bijgewerkt. Deze tasks lezen en schrijven “tegelijkertijd” dezelfde globale teller. Dat is ongewenst bij deze shared resource. De toegang tot deze variabele moet daarom worden afgeschermd, zodat slechts één task tegelijk de variabele kan bijwerken: de variabele lezen, met één verhogen en vervolgens terugschrijven. Hiervoor wordt gebruikgemaakt van een mutex.

## 2. Analoog-digitaalconversie met de ADC

Voor het meten van spanningen wordt de on-chip ADC van de processor op de Arduino Due gebruikt. De ADC beschikt over acht analoge ingangen; de ingangsspanning kan op één kanaal tegelijkertijd worden geconverteerd. Voor de ADC zijn onder andere de volgende functies beschikbaar:

- `void adc_Init(void)`: initialiseert de ADC en hoeft maar één keer te worden gebruikt.
- `void adc_EnableChannel(uint8_t channel)`: activeert het opgegeven ADC-kanaal.
- `void adc_StartConversion(void)`: start de AD-conversie op de kanalen die zijn geactiveerd.
- `bool adc_IsConversionReady(uint8_t channel)`: retourneert `true` als de conversie van het betreffende kanaal klaar is en `false` als de ADC nog bezig is met de conversie. De ADC-waarde die met `adc_ReadData` wordt uitgelezen, is uitsluitend geldig wanneer de conversie klaar is.
- `uint32_t adc_ReadData(uint8_t channel)`: retourneert de 12-bits ADC-waarde van het opgegeven kanaal.

In de volgende opdrachten worden de spanningen gemeten die worden ingesteld met de twee instelpotmeters op het RTSW-shield.

### Opdracht

Maak in de volgende opdracht gebruik van de solution `RTSW_week_3_ADC_Framework.sln`. Voeg uitsluitend code toe aan, of wijzig code in, het bestand `main.c`. Alle overige folders bevatten bestanden voor de systeemconfiguratie. Laat deze **ongewijzigd**.

De ADC lijkt in deze programma’s schijnbaar correct te functioneren en goede waarden te meten. Dit is echter niet het geval.

Een andere oplossing voor dit principiële probleem is het toepassen van semaforen. Dit wordt in de volgende opdracht toegepast.

## 3. Synchronisatie van tasks

Mutexes worden gebruikt voor mutual exclusion. Ook semaforen kunnen hiervoor worden gebruikt. Een semafoor met maximale waarde 1, ook wel een binaire semafoor genoemd, is functioneel hetzelfde als een mutex. Semaforen hebben echter veel andere toepassingen, bijvoorbeeld het synchroniseren van tasks. In de volgende opdrachten worden ze hiervoor gebruikt.

Gegeven zijn twee tasks:

- De task met de naam `TaskADC` leest elke seconde de analoge waarde uit van ADC-kanaal 0 en slaat de gemeten waarde op in de globale variabele `G_ADCValue`.
- De task met de naam `TaskDisplay` toont de waarde van `G_ADCValue` in Termite.

De synchronisatie van deze taken gebeurt met twee semaforen, zodat de volgorde van het meten van de ingangsspanning en het weergeven daarvan is gesynchroniseerd: eerst meten, daarna weergeven, enzovoort.

- De task `TaskDisplay` wacht op de semafoor `semaDisplay`. Deze wordt vrijgegeven door `TaskADC`, zodra deze task de waarde van `G_ADCValue` heeft bijgewerkt.
- De task `TaskADC` wacht op de semafoor `semaADC` voor een volgende meting. Deze wordt vrijgegeven zodra de task `TaskDisplay` de waarde van `G_ADCValue` heeft weergegeven.
- Bij het starten van beide tasks moet `TaskADC` eerst beginnen: eerst een meting uitvoeren en daarna pas de data weergeven. `TaskDisplay` moet wachten op beschikbare data, aangegeven door de semafoor `semaDisplay`. De initiële waarde van `semaDisplay` is daarom 0 en de initiële waarde van `semaADC` is 1.

### Opdracht

Maak in de volgende opdracht gebruik van de solution `RTSW_week_3_ThreadSync_Framework.sln`. Voeg uitsluitend code toe aan, of wijzig code in, het bestand `main.c`. Alle overige folders bevatten bestanden voor de systeemconfiguratie. Laat deze **ongewijzigd**.