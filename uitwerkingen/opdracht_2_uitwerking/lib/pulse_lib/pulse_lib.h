/*
 * pulse_lib.h
 *
 * Revision: V2.0
 * Modified: 07-09-2026
 */ 

#ifndef PULSE_LIB_H
#define PULSE_LIB_H

#include <Arduino.h>


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

