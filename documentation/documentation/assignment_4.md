# Opdracht 4: Producers, consumers en queues in FreeRTOS

In dit practicum komen de volgende onderwerpen aan bod:

- Het gebruik van queues voor communicatie tussen tasks in FreeRTOS
- Het genereren van analoge signalen met een digitaal-analoogconverter (DAC)
- Het bepalen van posities met een quadratuur-encoder

## 1. Producers, consumers en queues

In voorgaande practica werd data tussen tasks uitgewisseld met globale variabelen, of werd een gemeenschappelijke globale variabele door twee of meer tasks gewijzigd. Iedere task kan een globale variabele lezen en/of schrijven. Dat betekent dat de toegang tot zo’n variabele altijd moet worden afgeschermd met bijvoorbeeld een mutex. Een groot nadeel, of beter gezegd probleem, is dat elke variabele door alle tasks kan worden gewijzigd.

Uitwisseling van data tussen tasks in FreeRTOS kan efficiënter en veiliger worden geïmplementeerd met zogenoemde queues (wachtrijen). Aan een queue kan data worden toegevoegd door een zogenoemde producer (producent van data). De data wacht vervolgens in de queue op verwerking door een zogenoemde consumer (gebruiker van data).

In dit practicum is de producer een task die een ADC uitleest en de ADC-datasamples in een queue zet. De consumer is een task die wacht totdat er een vooraf vastgesteld aantal datasamples in de queue staat en vervolgens het gemiddelde van die samples bepaalt.

Daarnaast wordt een task gemaakt die de queue monitort en continu laat zien hoeveel data, in dit geval ADC-meetwaarden, in de queue staat.

### Opdracht

Maak in de volgende opdracht gebruik van de solution `RTSW_week_4_Queue_Framework.sln`. Voeg uitsluitend code toe aan, of wijzig code in, het bestand `main.c`. Alle overige folders bevatten bestanden voor de systeemconfiguratie. Laat deze **ongewijzigd**.

Om regelmatig weer te geven hoeveel elementen in de queue staan, wordt een monitor-task toegevoegd die elke 200 ms het aantal elementen in Termite weergeeft. Deze task wijzigt dus niet de inhoud van de queue, maar geeft uitsluitend de status weer.

In bovenstaande opdracht raakt de queue net vol: zodra de producer vijf samples in de queue heeft gezet, leest de consumer deze weer uit. De queue is dus weer leeg als het volgende sample beschikbaar komt.

## 2. Genereren van een analoog signaal met een digitaal-analoogconverter

Analoge signalen in een mechatronisch regelsysteem, bijvoorbeeld voor de aansturing van een motor, worden gemaakt met een digitaal-naar-analoogconverter (DAC). De hardware in dit practicum bevat vier analoge uitgangskanalen, waarvan de spanning instelbaar is tussen -10 volt en +10 volt. Deze spanning wordt gegenereerd door een 12-bits DAC met een bijbehorende uitgangsversterker.

**Figuur 1.** Digitaal-analoogconverter (DAC)

De relatie tussen de digitale 12-bits ingangswaarde $N$ van de DAC en de uitgangsspanning $V_{out}$ wordt gegeven door:

$$
V_{out} = -10 + \frac{N}{4096} \cdot 20\ \mathrm{V}, \qquad 0 \leq N \leq 4095
$$

De minimale uitgangsspanning is -10,000 volt voor $N = 0$. De maximale uitgangsspanning is 9,995 volt voor $N = 4095$, dus net geen +10 volt. Voor $N = 2048$ is de uitgangsspanning uiteraard 0 volt.

In de volgende opdracht wordt de DAC aangestuurd om een analoge golfvorm te maken op DAC-kanaal 0. De uitgangsspanning van DAC-kanaal 0 is beschikbaar op de BNC-connector `ANAOUT0` (connector `J2` op het board). Sluit deze met een coaxkabel aan op een oscilloscoop om het signaal te bekijken.

> **Let op:** ten opzichte van de processorsnelheid van 84 MHz, met een klokperiode van ongeveer 12 ns, heeft de DAC een relatief lange settling time van ongeveer 5 µs. De versterker heeft bovendien een relatief lage slew rate van ongeveer 0,5 V/µs. Opeenvolgende DAC-waarden mogen daarom niet te snel achter elkaar naar de DAC worden gestuurd; anders worden ze niet correct geconverteerd naar de bijbehorende analoge uitgangsspanning.
>
> Een geschikte wachttijd tussen het aanbieden van opeenvolgende DAC-waarden is ongeveer 20 µs. Gebruik hiervoor de FreeRTOS-functie `delay_us()`. Deze functie maakt **geen** gebruik van de FreeRTOS-scheduler. Het proces waarin deze functie wordt aangeroepen, wordt dus niet in de ready-queue geplaatst.

### Opdracht

Maak in deze opdracht gebruik van de solution `RTSW_week_4_DAC_Framework.sln`. Voeg uitsluitend code toe aan, of wijzig code in, het bestand `main.c`. Alle overige folders bevatten bestanden voor de systeemconfiguratie. Laat deze **ongewijzigd**.

## 3. Nauwkeurige positiebepaling met een quadratuur-encoder

Voor het bepalen van posities bij bewegende mechatronische systemen wordt vaak gebruikgemaakt van zogenoemde quadratuur-encoders of positieopnemers. Deze encoders genereren tijdens een beweging twee digitale signalen. De onderlinge fase van deze signalen geeft de draai- of bewegingsrichting aan, bijvoorbeeld linksom of rechtsom.

De positie van een as of opnemer wordt weergegeven door de waarde van een teller. Afhankelijk van de draai- of bewegingsrichting wordt de teller met één verhoogd of verlaagd. Nauwkeurige hoek- en positiebepaling is mogelijk; typische waarden zijn bijvoorbeeld 5.000 counts per omwenteling.

**Figuur 1.** Quadratuursignalen  
Bron: [Dynapar: Quadrature Encoder Basics](https://www.dynapar.com/technology/encoder_basics/quadrature_encoder/)

Daarnaast levert een quadratuur-encoder meestal nog een zogenoemd indexsignaal, dat een specifieke positie markeert, bijvoorbeeld bij een volledige rotatie of bij het bereiken van een referentiepositie.

Het optellen en aftellen van deze quadratuursignalen wordt volledig in hardware uitgevoerd met specifieke IC’s. De processor heeft hier geen bemoeienis mee. De software in FreeRTOS kan zich daarom beperken tot het eenmalig configureren en op geschikte momenten uitlezen van de waarden van de teller. Het moment van uitlezen wordt bepaald door de applicatie. Omdat het tellen in hardware gebeurt, worden er nooit counts gemist.

Omdat quadratuur-encoders meestal mechanisch aan een opstelling zijn gekoppeld, wordt in het practicum gebruikgemaakt van een quadratuur-interface. Hierbij wordt de encoder gesimuleerd met een draaiknop die de quadratuursignalen A en B genereert. Het indexsignaal wordt gegenereerd door de draaiknop in te drukken. Deze interface kan daarnaast ook worden gebruikt om rechtstreeks een quadratuur-encoder aan te sluiten.

### Opdracht

Maak in de volgende opdracht gebruik van de solution `RTSW_week_4_Quadrature_Framework.sln`. Voeg uitsluitend code toe aan, of wijzig code in, het bestand `main.c`. Alle overige folders bevatten bestanden voor de systeemconfiguratie. Laat deze **ongewijzigd**.