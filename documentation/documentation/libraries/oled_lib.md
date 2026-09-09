# OLED Display Library

## Overzicht

`oledDisplay` is een eenvoudige driver voor een SSD1306 OLED-display. De bibliotheek maakt het mogelijk om tekstregels te schrijven en het scherm te wissen.

## API

```cpp
class oledDisplay {
public:
  bool Init(void);
  void Clear(void);
  void WriteLine(uint8_t line, const char *message, uint8_t align);
};
```

## Constanten

```cpp
#define ALIGN_LEFT   0
#define ALIGN_RIGHT  1
#define ALIGN_CENTER 2

#define OLED_NLINES 4
#define OLED_XSIZE  128
#define OLED_YSIZE  64
```

## Gebruik

```cpp
oledDisplay display;

display.Init();
display.Clear();
display.WriteLine(0, "Hello", ALIGN_CENTER);
```

## Opmerkingen

- Het display is ontworpen voor 128x64 SSD1306-schermen.
- `WriteLine()` ondersteunt links/centrum/rechts uitlijnen.
