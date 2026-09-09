# SPI-bibliotheek

## Overzicht

`spi_device` is de centrale SPI-abstractie in deze bibliotheekset. De bibliotheek beheert de VSPI-bus, device-selectie via een 74HC138 MUX en basisgegevensoverdracht naar SPI-slaves.

## API

```cpp
class spi_device {
public:
    void Init(void);
    void BeginTransaction(SPISettings settings, uint8_t spiDeviceNumber);
    void EndTransaction(void);

    void WriteByte(const uint8_t data);
    void WriteWord(const uint16_t data);
    void ReadByte(uint8_t *byteData);
    void ReadWord(uint16_t *wordData);
    void SelectDevice(uint8_t spiDeviceNumber);
    void DeselectDevice(void);
    uint8_t TransferByte(uint8_t byteToSend);
    uint16_t TransferWord(uint16_t wordToSend);
};
```

## Belangrijkste constanten

```cpp
#define SPI_MAX_DEVICENUMBER 7
#define SPI_N_SELECTBITS 3
#define SPI_DEFAULT_SPEED 4000000
```

## Geselecteerde devices

- `SPI_DEVICE_DAC01` = 0
- `SPI_DEVICE_DAC23` = 1
- `SPI_DEVICE_QC0` = 2
- `SPI_DEVICE_QC1` = 3
- `SPI_DEVICE_ADC` = 4
- `SPI_DEVICE_EXT_5` = 5
- `SPI_DEVICE_EXT_6` = 6
- `SPI_DEVICE_UNUSED` = 7

## Gebruik

```cpp
spi_device spi;
spi.Init();
spi.SelectDevice(SPI_DEVICE_ADC);
spi.WriteWord(0x1234);
spi.DeselectDevice();
```

## Opmerkingen

- De MUX-selectpinnen zijn vastgelegd als `GPIO_NUM_5`, `GPIO_NUM_17` en `GPIO_NUM_16`.
- De SPI-bus gebruikt `VSPI` op de ESP32.
