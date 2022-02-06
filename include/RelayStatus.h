#pragma once

#include "NTPClient.h"

class RelayStatus {
   public:
    RelayStatus(const int pin, const char* name, const char* nvsKeyName, NTPClient& clock)
        : _pin(pin),
          _name(name),
          _nvsKeyName(nvsKeyName),
          _clock(clock),
          _state(false),
          _lastOn(0),
          _lastOff(0),
          _isAutocontrolled(true) {
        pinMode(_pin, OUTPUT);
        setRelay(false);
    };

    String toJson() const {
        PrintfString str;
        str.printf("\"%s\":{\"lastOn\":%lu,\"lastOff\":%lu,\"state\":%s,\"autocontrolled\":%s}", _name, _lastOn,
                   _lastOff, _state ? "true" : "false", _isAutocontrolled ? "true" : "false");
        return str;
    };

    const String getName() const { return _name; };

    void setAutoControlled(boolean state) { _isAutocontrolled = state; };

    const boolean isAutoControlled() const { return _isAutocontrolled; };

    const char* getNvsKeyName() const { return _nvsKeyName; };

    void setRelay(boolean relayState) {
        if (!_state && relayState) {
            _lastOn = _clock.getEpochTime();
        } else if (_state && !relayState) {
            _lastOff = _clock.getEpochTime();
        }
        digitalWrite(_pin, relayState ? LOW : HIGH);
        _state = relayState;
    }

    void setRelay(HysteresisAction relayState) {
        switch (relayState) {
            case ON:
                setRelay(true);
                break;
            case OFF:
                setRelay(false);
                break;
            default:
                break;
        }
    }

    boolean getState() const { return _state; }

   private:
    int _pin;
    const char* _name;
    const char* _nvsKeyName;
    NTPClient& _clock;
    boolean _state;
    unsigned long _lastOn, _lastOff;
    boolean _isAutocontrolled;
};
