/*
 * pulse_lib.cpp
 *
 * Revision: V2.0
 * Modified: 07-09-2026
 */ 

#include "pulse_lib.h"

PulseLib::PulseLib()
		: _pin(-1), _pulsing(false), _asyncActive(false), _outputHigh(false),
			_pulseWidthMs(0), _pauseWidthMs(0), _pulseCount(0), _currentPulse(0),
			_lastToggle(0) {}

// configures pin as output, driven LOW, ready for pulsing
void PulseLib::Begin(int pin) {
	_pin = pin;
	_pulsing = false;
	_asyncActive = false;
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
}

// blocking: drives a single HIGH pulse of duration_ms then returns to LOW
void PulseLib::Pulse(int duration_ms) {
	if (_pin < 0) {
		return;
	}
	_pulsing = true;
	digitalWrite(_pin, HIGH);
	delay(duration_ms);
	digitalWrite(_pin, LOW);
	_pulsing = false;
}

// non-blocking: starts a single HIGH pulse, completion is driven by Tick()
void PulseLib::PulseAsync(int duration_ms) {
	if (_pin < 0 || duration_ms < 0) {
		return;
	}
	_pulseWidthMs = duration_ms;
	_pauseWidthMs = 0;
	_pulseCount = 1;
	_currentPulse = 0;
	_outputHigh = false;
	_asyncActive = true;
	_pulsing = true;
	_lastToggle = millis();

	// Start the Pulse
	digitalWrite(_pin, HIGH);
	_outputHigh = true;
	_lastToggle = millis();
	++_currentPulse;
}

// true while a blocking or async pulse sequence is in progress
bool PulseLib::IsPulsing() {
	return _pulsing;
}

// immediately forces the pin LOW and cancels any active pulse sequence
void PulseLib::StopPulse() {
	if (_pin >= 0) {
		digitalWrite(_pin, LOW);
	}
	_pulsing = false;
	_asyncActive = false;
}

// blocking: generates pulseCount HIGH/LOW pulses separated by pauseWidthMs
void PulseLib::GeneratePulses(int pulseWidthMs, int pauseWidthMs, int pulseCount) {
	if (pulseCount <= 0 || pulseWidthMs < 0 || pauseWidthMs < 0) {
		return;
	}


	for (int i = 0; i < pulseCount; ++i) {
		Pulse(pulseWidthMs);
		if (i < pulseCount - 1) {
			delay(pauseWidthMs);
		}
	}
}

// non-blocking: arms a pulse train, timing is advanced by repeated Tick() calls
void PulseLib::GeneratePulsesAsync(int pulseWidthMs, int pauseWidthMs, int pulseCount) {
	if (pulseCount <= 0 || pulseWidthMs < 0 || pauseWidthMs < 0) {
		return;
	}


	_pulseWidthMs = pulseWidthMs;
	_pauseWidthMs = pauseWidthMs;
	_pulseCount = pulseCount;
	_currentPulse = 0;
	_outputHigh = false;
	_asyncActive = true;
	_pulsing = true;
	_lastToggle = millis();

	// ensure starting from LOW
	digitalWrite(_pin, LOW);
}

// call repeatedly (e.g. from loop()) to advance an async pulse train without blocking
void PulseLib::Tick() {
	if (!_asyncActive) {
		return;
	}

	unsigned long now = millis();
	unsigned long elapsed = now - _lastToggle;

	if (_outputHigh) {
		if (elapsed >= static_cast<unsigned long>(_pulseWidthMs)) {
			digitalWrite(_pin, LOW);
			_outputHigh = false;
			_lastToggle = now;
		}
	} else {
		if (_currentPulse >= _pulseCount) {
			_asyncActive = false;
			_pulsing = false;
			return;
		}

		if (elapsed >= static_cast<unsigned long>(_pauseWidthMs)) {
			digitalWrite(_pin, HIGH);
			_outputHigh = true;
			_lastToggle = now;
			++_currentPulse;
		}
	}
}

// number of pulses left to emit in the current async pulse train
int PulseLib::GetRemainingPulses() {
	if (!_asyncActive) {
		return 0;
	}

	int remaining = _pulseCount - _currentPulse;
	if (remaining < 0) {
		remaining = 0;
	}
	return remaining;
}
