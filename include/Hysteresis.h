#pragma once

#include "CircularBuffer.h"
#include "PrintfString.h"
#include "Serializable.h"

enum HysteresisAction { ON, OFF, DO_NOTHING };

class HysteresisConfigData {
public:
	HysteresisConfigData() : targetValue(-1), delta(-1), wasInitialized(false) {}
	HysteresisConfigData(int _targetValue, int _delta) : targetValue(_targetValue), delta(_delta), wasInitialized(true) {};

	int targetValue;
	int delta;
	bool wasInitialized;
};

class Hysteresis : public Serializable {
public:
	Hysteresis(const char *name,
			   bool invertedValues = false) // invertedValues=false for heating, invertedValues=true for cooling
		: Serializable(name), _currentState(false), _invertedValues(invertedValues) {};

	void storeState() {
		nvs.setInt("targetValue", (int32_t)_config.targetValue);
		nvs.setInt("delta", (int32_t)_config.delta);
		nvs.commit();
	};
	boolean retrieveState() {
		int64_t value = nvs.getInt("targetValue", LONG_MIN);
		if (value == LONG_MIN) {
			return false;
		}
		_config.targetValue = value;

		value = nvs.getInt("delta", LONG_MIN);
		if (value == LONG_MIN) {
			return false;
		}
		_config.delta = value;
		_config.wasInitialized = true;
		return true;
	};

	void reset(const JsonObject &json) {
		_config.targetValue = json["target"];
		_config.delta = json["delta"];

		_config.wasInitialized = true;
	}

	void reset(const HysteresisConfigData &config) {
		_config = config;
		_config.wasInitialized = true;
	}

	int getAverageValue() {
		if (!_buffer.isFull()) {
			return -1;
		}

		int average = 0;
		for (buffer_index_t i = 0; i < _buffer.size(); i++) {
			average += _buffer[i];
		}
		average /= _buffer.size();
		return average;
	}

	HysteresisAction checkValue(float value) {
		if (!_config.wasInitialized || isnan(value)) {
			return DO_NOTHING;
		}

		_buffer.push(value);
		if (!_buffer.isFull()) {
			return DO_NOTHING;
		}

		int average = getAverageValue();

		int lowBoundary = _config.targetValue - _config.delta;
		int highBoundary = _config.targetValue + _config.delta;
		if (!_invertedValues) {
			if (_currentState) {
				if (average > highBoundary) {
					_currentState = false;
				}
			} else {
				if (average < lowBoundary) {
					_currentState = true;
				}
			}
		} else {
			if (_currentState) {
				if (average < lowBoundary) {
					_currentState = false;
				}
			} else {
				if (average > highBoundary) {
					_currentState = true;
				}
			}
		}

		return _currentState ? HysteresisAction::ON : HysteresisAction::OFF;
	}

	String toJson() const {
		PrintfString str;
		str.printf("{\"WasInitialized\":%s", _config.wasInitialized ? "true" : "false");
		if (_config.wasInitialized) {
			PrintfString content;
			content.printf(",\"target\":%i,\"delta\":%i,\"state\":%s", _config.targetValue, _config.delta,
						   _currentState ? "true" : "false");
			str += content;
		}
		str += "}";
		return str;
	}

	int getTargetValue() { return _config.targetValue; };

private:
	bool _currentState;
	HysteresisConfigData _config;
	CircularBuffer<int, 5> _buffer;
	bool _invertedValues;
	using buffer_index_t = decltype(_buffer)::index_t;
};
