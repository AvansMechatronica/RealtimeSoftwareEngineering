# Button Library

## Overzicht

`button` is een kleine helper-library voor het beheren van digitale en analoge drukknoppen. De library gebruikt een `adc3208`-interface om analoge knopstatussen te detecteren.

## Kenmerken

- ondersteuning voor 3 knoppen
- 1 digitale knop
- 2 analoge knoppen via ADC-kanalen
- eenvoudige statuscheck via `IsPressed()`

## API

```cpp
class button {
public:
    button();
    void Init(adc3208 *adc);
    bool IsPressed(uint8_t buttonNumber);
};
```

## Constanten

```cpp
#define BUTTON_PIN GPIO_NUM_4
#define N_BUTTONS 3
```

## Gebruik

```cpp
spi_device spi;
adc3208 adc;
button btn;

spi.Init();
adc.Init(&spi);
btn.Init(&adc);

if (btn.IsPressed(0)) {
    // knop 0 ingedrukt
}
```

## Opmerkingen

- `buttonNumber` verwacht een index in het bereik `0` tot `N_BUTTONS - 1`.
- De driver is ontworpen voor een ESP32 board met een gecombineerde digitale en analoge input-structuur.
