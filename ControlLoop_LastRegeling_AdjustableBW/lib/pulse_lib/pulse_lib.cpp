#include "pulse_lib.h"

PulseLib::PulseLib()
		: _pin(-1), _pulsing(false), _asyncActive(false), _outputHigh(false),
			_pulseWidthMs(0), _pauseWidthMs(0), _pulseCount(0), _currentPulse(0),
			_lastToggle(0) {}

void PulseLib::begin(int pin) {
	_pin = pin;
	_pulsing = false;
	_asyncActive = false;
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
}

void PulseLib::pulse(int duration_ms) {
	if (_pin < 0) {
		return;
	}
	_pulsing = true;
	digitalWrite(_pin, HIGH);
	delay(duration_ms);
	digitalWrite(_pin, LOW);
	_pulsing = false;
}

void PulseLib::pulseAsync(int duration_ms) {
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

	// Start the pulse
	digitalWrite(_pin, HIGH);
	_outputHigh = true;
	_lastToggle = millis();
	++_currentPulse;
}

bool PulseLib::isPulsing() {
	return _pulsing;
}

void PulseLib::stopPulse() {
	if (_pin >= 0) {
		digitalWrite(_pin, LOW);
	}
	_pulsing = false;
	_asyncActive = false;
}

void PulseLib::generetePulses(int pulseWidthMs, int pauseWidthMs, int pulseCount) {
	if (pulseCount <= 0 || pulseWidthMs < 0 || pauseWidthMs < 0) {
		return;
	}


	for (int i = 0; i < pulseCount; ++i) {
		pulse(pulseWidthMs);
		if (i < pulseCount - 1) {
			delay(pauseWidthMs);
		}
	}
}

void PulseLib::generetePulsesAsync(int pulseWidthMs, int pauseWidthMs, int pulseCount) {
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

void PulseLib::tick() {
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

int PulseLib::getRemainingPulses() {
	if (!_asyncActive) {
		return 0;
	}

	int remaining = _pulseCount - _currentPulse;
	if (remaining < 0) {
		remaining = 0;
	}
	return remaining;
}
