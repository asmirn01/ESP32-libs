#pragma once

#include "ArduinoNvs.h"
#include "PrintfString.h"
#include "Syslog.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "OptionalValue"

#define MAX_NVS_KEY_LENGTH 15

extern Syslog syslog;

template <typename T>
class OptionalValue {
   public:
    OptionalValue(const char* nvsKeyName, const T& defValue) : _defValue(defValue) {
        if (strlen(nvsKeyName) >= (MAX_NVS_KEY_LENGTH - 1)) {
            ESP_LOGE(TAG, "Too long NVS key name '%s'", nvsKeyName);
        }
        strncpy(_nvsKeyName, nvsKeyName, MAX_NVS_KEY_LENGTH);
        strcpy(_nvsIsValidKeyName, "_");
        strncat(_nvsIsValidKeyName, nvsKeyName, MAX_NVS_KEY_LENGTH - 1);
    }

    OptionalValue& operator=(const T& v) {
        setValue(v);
        return *this;
    };
    void setValue(const T& value) {
        _value = value;
        _isValid = true;
    }

    void setInvalid() { _isValid = false; }

    boolean isValid() const { return _isValid; }

    T value() const { return _value; }

    operator const T&() const { return _value; }

    String toJson() const {
        PrintfString str;
        str.printf("{\"isSet\":%s, \"value\":%s}", _isValid ? "true" : "false",
                   _isValid ? String(_value).c_str() : "null");
        return str;
    }

    void printValueOrNull(Stream& serial) const {
        if (!_isValid) {
            serial.print(F("null"));
        } else {
            serial.print(_value);
        }
    }

    String valueOrNull() const {
        if (!_isValid) {
            return "null";
        } else {
            return String(_value);
        }
    }

    bool load(ArduinoNvs& nvs) {
        int64_t isValid = nvs.getInt(_nvsIsValidKeyName, 0);
        if (isValid == 0) {
            setInvalid();
            ESP_LOGD(TAG, "Reading %s as invalid", _nvsKeyName);
            return false;
        } else {
            setValue(nvs.getInt(_nvsKeyName));
            ESP_LOGD(TAG, "Reading %s as valid %s", _nvsKeyName, valueOrNull().c_str());
            return true;
        }
    }

    void loadOrDefault(ArduinoNvs& nvs) {
        load(nvs);
        if (!isValid()) {
            setValue(_defValue);
        }
    }

    void save(ArduinoNvs& nvs) const {
        if (_isValid) {
            ESP_LOGD(TAG, "Saving %s as valid %s", _nvsKeyName, valueOrNull().c_str());
            nvs.setInt(_nvsIsValidKeyName, (uint8_t)1);
            nvs.setValue(_nvsKeyName, _value);
        } else {
            ESP_LOGD(TAG, "Saving %s as invalid", _nvsKeyName);
            nvs.setInt(_nvsIsValidKeyName, (uint8_t)0);
        }
    }

   private:
    T _value;
    T _defValue;
    boolean _isValid;
    char _nvsKeyName[MAX_NVS_KEY_LENGTH + 1] = {0};
    char _nvsIsValidKeyName[MAX_NVS_KEY_LENGTH + 1] = {0};
};
