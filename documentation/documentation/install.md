# Visual Studio Code en PlatformIO

In deze sectie lees je hoe je Visual Studio Code (VS Code) en PlatformIO installeert voor het ontwikkelen van microcontrollerprojecten.

## Installeren van Visual Studio Code

1. Ga naar de officiele website: [https://code.visualstudio.com/](https://code.visualstudio.com/).
2. Download de versie voor jouw besturingssysteem.
3. Volg de installatiestappen op het scherm.

## Installeren van de PlatformIO-extensie

1. Open Visual Studio Code.
2. Open Extensions via het zijbalk-icoon of met `Ctrl+Shift+X`.
3. Zoek op `PlatformIO IDE`.
4. Klik op `Install`.

## PlatformIO gebruiken in VS Code

1. Klik na installatie op het PlatformIO-icoon in de zijbalk.
2. Open `PlatformIO Home`.
3. Kies `New Project` om een nieuw project te maken.

## Verkrijgen van de workshopbestanden

Om te starten heb je de workshopbestanden nodig met code, schema's en opdrachtinstructies.

:::::{card}
::::{tab-set}

:::{tab-item} Download ZIP
Download de bestanden als ZIP via:
[Download Workshop Bestanden](https://github.com/AvansMechatronica/Microcontrollers_workshop/archive/refs/heads/main.zip)

Pak het ZIP-bestand uit in een map die je makkelijk terugvindt.

:::

:::{tab-item} Git Clone

Als je al bekend bent met Git en github, kun je de bestanden ook via Github verkrijgen. Volg de onderstaande stappen:

* Maak een account aan bij [Github](https://github.com/) en login op dit account

* Open de [Microcontrollers_workshop](https://github.com/AvansMechatronica/Microcontrollers_workshop) repository

* Maak een Fork van de repository naar je eigen Github account door op het **Fork icoon**  te klikken:

![image](../images/fork.jpg)

* Volg de instructies, maar wijzig de naam van de nieuwe repository niet. Bevestig met **Create Fork**  

* Nu kun je de workshop clonen naar je lokale machine:

```bash
git clone https://github.com/<jouw_account_naam>/Microcontrollers_workshop.git
```

:::

::::

:::::

:::{warning}
Plaats de bestanden niet in een `OneDrive`-map. Dit kan compileerproblemen veroorzaken.
:::