# Digitale I/O-bibliotheek

## Overzicht

`dio_device` is een compacte bibliotheek voor het beheren van digitale ingangen en uitgangen op een ESP32. De bibliotheek definieert vaste GPIO-pinnen voor input- en output-bitvelden.

## API

```cpp
class dio_device {
public:
    void Init(void);
    uint8_t GetInput(void);
    bool IsBitSet(uint8_t bitNumber);
    void SetOutput(uint8_t value);
    void SetBit(uint8_t bitNumber);
    void ClearBit(uint8_t bitNumber);
    void ToggleBit(uint8_t bitNumber);
    int16_t GetGPIONumberInput(uint8_t inputBitNumber);
};
```

## Constanten

```cpp
#define N_INPUT_BITS  6
#define N_OUTPUT_BITS 6
```

## In- en uitgangspinnen

- Inputs: `GPIO_NUM_36`, `39`, `34`, `35`, `32`, `33`
- Outputs: `GPIO_NUM_25`, `26`, `27`, `14`, `12`, `13`

## Gebruik

```cpp
dio_device io;
io.Init();

uint8_t inputs = io.GetInput();
if (io.IsBitSet(0)) {
    io.SetBit(2);
}
```

## Opmerkingen

- De bibliotheek ondersteunt 6 digitale inputbits en 6 outputbits.
- `SetOutput()` kan een volledig byte-achtig patroon sturen naar alle outputs.
