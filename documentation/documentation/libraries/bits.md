# Diversen bitdefinities

## Overzicht

`bits.h` bevat een compacte set macro-definities voor individuele bitmaskers. De bibliotheek is bedoeld om bitcontrole in embedded C/C++-code te vereenvoudigen.

## API

```cpp
#define _BV(bit) (1UL << (bit))
#define BIT_0  _BV(0)
#define BIT_1  _BV(1)
...
#define BIT_15 _BV(15)
```

## Gebruik

```cpp
uint16_t control = BIT_10 | BIT_12;
if (control & BIT_10) {
    // bit 10 is actief
}
```

## Doel

Deze helper is veel gebruikt door de ADC-, DAC- en quadratuur-counter-libraries in deze set.
