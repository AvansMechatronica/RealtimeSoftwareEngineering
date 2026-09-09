# ADC 3208 Library

## Overzicht

`adc3208` is a driver for the MCP3208 8-channel analog-to-digital converter. The library is built for ESP32 and uses the shared SPI bus abstraction from the SPI library in this workspace.

## Kenmerken

- 8 analoge ingangen
- 12-bit resolutie
- uitlezen van ruwe waarden en spanning
- meervoudige kanaalmetingen in één aanroep
- ondersteuning voor analoge drukknoppen

## API

```cpp
class adc3208 {
public:
    void Init(spi_device *spi_bus);
    uint16_t ReadRaw(uint8_t channel, uint8_t averageCount = 1);
    void ReadRawMultiple(uint8_t channelList[], uint8_t numChannels, uint16_t rawValues[]);
    void ReadVoltageMultiple(uint8_t channelList[], uint8_t numChannels, double voltages[]);
    double ReadVoltage(uint8_t channel, uint8_t averageCount = 1);
    bool IsButtonPressed(uint8_t analogButton);
};
```

## Belangrijkste constanten

- `N_ADC_CHANNELS` = 8
- `N_ADC_BITS` = 12
- `ADC_REFERENCE_VOLTAGE` = 2.5 V
- `SPI_ADC_SPEED` = 2000000

## Gebruik

```cpp
spi_device spi;
adc3208 adc;

spi.Init();
adc.Init(&spi);

uint16_t raw = adc.ReadRaw(0);
double voltage = adc.ReadVoltage(0);
```

## Opmerkingen

- Kanaal 0 t/m 3 zijn bedoeld voor invoer in het bereik van ongeveer -10 V tot +10 V.
- Kanaal 4 t/m 7 zijn bedoeld voor 0 V tot 2.5 V.
- De library gebruikt `fmap.h`, `bits.h` en de SPI helper vanuit deze bibliotheekset.
