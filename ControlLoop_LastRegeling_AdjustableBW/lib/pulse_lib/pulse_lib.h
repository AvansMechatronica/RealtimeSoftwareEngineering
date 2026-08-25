#ifndef PULSE_LIB_H
#define PULSE_LIB_H

#include <Arduino.h>


class PulseLib {    
  public:
    PulseLib();
    void begin(int pin);
    void pulse(int duration_ms);
    void pulseAsync(int duration_ms);
    bool isPulsing();
    void stopPulse();

    void generetePulses(int pulseWidthMs, int pauseWidthMs, int pulseCount);
    void generetePulsesAsync(int pulseWidthMs, int pauseWidthMs, int pulseCount);
    void tick();
    int getRemainingPulses();
    
  private:
    int _pin;
    bool _pulsing;
    bool _asyncActive;
    bool _outputHigh;
    int _pulseWidthMs;
    int _pauseWidthMs;
    int _pulseCount;
    int _currentPulse;
    unsigned long _lastToggle;
};  


#endif

