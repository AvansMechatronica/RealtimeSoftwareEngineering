# LED-bibliotheek

## Overzicht

`led` is een driver voor het aansturen van LED's op een ESP32-board. De bibliotheek ondersteunt een vast aantal LED-kanalen en valideert de index voordat een GPIO wordt bediend.

## API

```cpp
class led {
public:
    led();
    void Init(void);
    void Set(uint8_t ledNumber, bool ledOn);
};
```

## Constanten

```cpp
#define N_LEDS 2
#define LED_BLUE 0
#define LED_IO15 1
```

## Gebruik

```cpp
led statusLed;
statusLed.Init();
statusLed.Set(LED_BLUE, true);
```

## Opmerkingen

- `LED_PCB` wordt standaard gedefinieerd als `GPIO_NUM_2`.
- De bibliotheek kan hiermee eenvoudig worden aangepast voor een custom board-pin-layout via `LED_PIN_ESP32_BOARD`.
