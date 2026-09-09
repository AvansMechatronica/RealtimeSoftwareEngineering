# Quadrature Counter Library

## Overzicht

`qc7366` is een driver voor de LS7366R quadrature counter. De library maakt het mogelijk om tellerregisters te initialiseren, te lezen, te wissen en te configureren.

## Kenmerken

- 2 kanalen
- configureerbare modusregisters
- teller- en statusregisters lezen
- counter enable/disable
- index-status controle

## API

```cpp
class qc7366 {
public:
    void Init(spi_device *spi_bus);
    void WriteModeRegister(uint8_t channel, mode_register_t modeRegister, uint8_t valueMDR);
    void ClearModeRegister(uint8_t channel, mode_register_t modeRegister);
    uint8_t ReadModeRegister(uint8_t channel, mode_register_t modeRegister);

    void ClearCountRegister(uint8_t channel);
    int32_t ReadCountRegister(uint8_t channel);

    void ClearStatusRegister(uint8_t channel);
    uint8_t ReadStatusRegister(uint8_t channel);

    void WriteDataRegister(uint8_t channel, int32_t dtrValue);
    void TransferDataRegisterToCountRegister(uint8_t channel);

    int32_t ReadOutputRegister(uint8_t channel);

    void EnableCounter(uint8_t channel);
    void DisableCounter(uint8_t channel);

    bool IsIndexSet(uint8_t channel);
};
```

## Belangrijkste constanten

- `QC_N_CHANNELS` = 2
- `SPI_QC_SPEED` = 4000000
- `MODE_NON_QC`, `MODE_QC_1`, `MODE_QC_2`, `MODE_QC_4`

## Gebruik

```cpp
spi_device spi;
qc7366 encoder;

spi.Init();
encoder.Init(&spi);

int32_t count = encoder.ReadCountRegister(0);
```

## Opmerkingen

- De library communiceert via de shared SPI device abstraction.
- De status- en mode-registers zijn specifiek voor de LS7366R-chip.
