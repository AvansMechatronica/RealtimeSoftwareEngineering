# Opdracht 1: Installatie van de software


## TODO: Kontroleer dze tekst
In dit practicum wordt uitgelegd hoe de benodigde software voor het werken met het RTSW-shield en FreeRTOS geïnstalleerd moet worden. Volg de onderstaande stappen om de installatie correct uit te voeren.

## 1. Installatie van de ontwikkelomgeving

1. Download en installeer de laatste versie van [Visual Studio](https://visualstudio.microsoft.com/).
2. Tijdens de installatie, zorg ervoor dat de workload voor C++-ontwikkeling is geselecteerd.

## 2. Installatie van de benodigde SDK's en tools

1. Download en installeer de STM32CubeIDE van [STMicroelectronics](https://www.st.com/en/development-tools/stm32cubeide.html).
2. Installeer de STM32CubeMX tool, indien deze niet standaard is meegeleverd met de STM32CubeIDE.

## 3. Configuratie van het project

1. Open de solution `RTSW_week_2_Framework.sln` in Visual Studio.
2. Controleer of alle projectinstellingen correct zijn en dat de juiste toolchain is geselecteerd.

## 4. Verifiëren van de installatie

1. Bouw het project om te controleren of alle dependencies correct zijn geïnstalleerd.
2. Sluit het RTSW-shield aan en programmeer het board met een eenvoudige LED-knipper test om te verifiëren dat de hardware correct werkt.