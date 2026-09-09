# DAC 4922 Bibliotheek

## Overzicht

`dac4922` is een bibliotheek voor de MCP4922 dual-output DAC. De bibliotheek ondersteunt het schrijven van ruwe waarden en het omzetten van spanningen naar een juiste DAC-waarde.

## Kenmerken

- 2 kanalen (A en B) of 4 kanaal-achtige kanalen volgens board-layout
- 12-bit resolutie
- instellen van spanning per kanaal
- batch-actie voor alle outputs

## API

```cpp
class dac4922 {
public:
    void Init(spi *spi_bus);
    void Write(uint8_t dacChannel, uint16_t dacValue);
    void SetOutputVoltage(uint8_t dacChannel, float outputVoltage);
    void SetOutputVoltageAll(float outputVoltage);
};
```

## Belangrijkste constanten

- `N_DAC_BITS` = 12
- `N_DAC_CHANNELS` = 4
- `DAC_SPAN` = 20.0 V
- `DAC_MIN_VOLTAGE` = -10.0 V
- `DAC_MAX_VOLTAGE` = +10.0 V
- `SPI_DAC_SPEED` = 10000000

## Gebruik

```cpp
spi bus;
dac4922 dac;

bus.Init();
dac.Init(&bus);

dac.SetOutputVoltage(0, 2.5f);
dac.SetOutputVoltage(1, -3.0f);
```

## Opmerkingen

- De library gebruikt een SPI-bus met mode 0 en MSB-first.
- De selectie van DAC-kanaal A/B gebeurt via bitinstellingen in het schrijfsignaal.
