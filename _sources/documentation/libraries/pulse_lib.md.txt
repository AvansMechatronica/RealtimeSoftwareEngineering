# Pulse-bibliotheek

## Overzicht

`PulseLib` biedt een flexibele API voor het genereren van puls-signalen op een GPIO-pin. Het ondersteunt zowel directe pulse-oproepen als asynchrone pulsgeneratie met timing.

## API

```cpp
class PulseLib {
public:
    PulseLib();
    void Begin(int pin);
    void Pulse(int duration_ms);
    void PulseAsync(int duration_ms);
    bool IsPulsing();
    void StopPulse();

    void GeneratePulses(int pulseWidthMs, int pauseWidthMs, int pulseCount);
    void GeneratePulsesAsync(int pulseWidthMs, int pauseWidthMs, int pulseCount);
    void Tick();
    int GetRemainingPulses();
};
```

## Gebruik

```cpp
PulseLib pulse;
pulse.Begin(GPIO_NUM_4);
pulse.Pulse(250);
```

## Doel

Deze bibliotheek is handig voor het sturen van timing-gevoelige output-signalen, zoals triggers, pulse-train uitvoer of actuator-activering.

## Opmerkingen

- `PulseAsync()` en `GeneratePulsesAsync()` zijn ontworpen voor non-blocking werking.
- `Tick()` wordt typisch in een loop of timer-omgeving aangeroepen om de state-machine voort te zetten.
