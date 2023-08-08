#pragma once

#include "CircularBuffer.h"
#include "PrintfString.h"
#include "Serializable.h"

enum HysteresisAction { ON, OFF, DO_NOTHING };

class HysteresisConfigData {
   public:
    HysteresisConfigData() : meanValue(-1), delta(-1), wasInitialized(false) {}
    HysteresisConfigData(int _meanValue, int _delta) : meanValue(_meanValue), delta(_delta), wasInitialized(true){};

    int meanValue;
    int delta;
    bool wasInitialized;
};

class Hysteresis : public Serializable {
   public:
    Hysteresis(const char* name) : Serializable(name), _currentState(false){};

    void storeState() {
        nvs.setInt("meanValue", (int32_t)_config.meanValue);
        nvs.setInt("delta", (int32_t)_config.delta);
        nvs.commit();
    };
    boolean retrieveState() {
        int64_t value = nvs.getInt("meanValue", LONG_MIN);
        if (value == LONG_MIN) {
            return false;
        }
        _config.meanValue = value;

        value = nvs.getInt("delta", LONG_MIN);
        if (value == LONG_MIN) {
            return false;
        }
        _config.delta = value;
        _config.wasInitialized = true;
        return true;
    };

    void reset(const JsonObject& json) {
        _config.meanValue = json["mean"];
        _config.delta = json["delta"];

        _config.wasInitialized = true;
    }

    void reset(const HysteresisConfigData& config) {
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

        if (_currentState) {
            if (average >= (_config.meanValue + _config.delta)) {
                _currentState = false;
            }
        } else {
            if (average < (_config.meanValue - _config.delta)) {
                _currentState = true;
            }
        }

        return _currentState ? HysteresisAction::ON : HysteresisAction::OFF;
    }

    String toJson() const {
        PrintfString str;
        str.printf("{\"WasInitialized\":%s", _config.wasInitialized ? "true" : "false");
        if (_config.wasInitialized) {
            PrintfString content;
            content.printf(",\"mean\":%i,\"delta\":%i,\"state\":%s", _config.meanValue, _config.delta,
                           _currentState ? "true" : "false");
            str += content;
        }
        str += "}";
        return str;
    }

    int getMeanValue() { return _config.meanValue; };

   private:
    bool _currentState;
    HysteresisConfigData _config;
    CircularBuffer<int, 5> _buffer;
    using buffer_index_t = decltype(_buffer)::index_t;
};
