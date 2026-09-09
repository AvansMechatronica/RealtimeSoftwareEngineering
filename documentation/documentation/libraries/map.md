# Map-/conversiebibliotheek

## Overzicht

Deze bibliotheek bevat basisfunctie-helpers voor het schalen van waarden van het ene bereik naar het andere. De functies zijn bedoeld als Arduino-achtige helpers voor mapping en begrenzing.

## API

```cpp
long int map(long int x, long int in_min, long int in_max,
             long int out_min, long int out_max);

float fmap(float x, float in_min, float in_max, float out_min, float out_max);
float constrain(float x, float min, float max);
```

## Gebruik

```cpp
float scaled = fmap(3.3f, 0.0f, 5.0f, 0.0f, 100.0f);
float limited = constrain(150.0f, 0.0f, 100.0f);
```

## Doel

- schaal waarden naar een nieuw bereik
- beperk waarden binnen een minimum- en maximumgrens
- gebruikelijk voor sensor- en actuator-calibratie
