#pragma once

#include "WString.h"

#define stringify(name) #name
enum MetricType { gauge, counter };
const char* MetricTypeNames[] = {[gauge] = stringify(gauge), [counter] = stringify(counter)};

class Metric {
   public:
    Metric(const String& _name, const String& _help, const MetricType _type,
           const unsigned int _expirationTimeMs = 5000, const unsigned int _decimals = 1)
        : name(_name), help(_help), type(_type), expirationTimeMs(_expirationTimeMs), decimals(_decimals){};
    void setValue(const int newValue) { setValue((float)newValue); }
    void setValue(const long newValue) { setValue((float)newValue); }
    void setValue(const unsigned long newValue) { setValue((float)newValue); }
    void setValue(const uint32_t newValue) { setValue((float)newValue); }
    void setValue(const float newValue) {
        value = newValue;
        lastSetTimeMs = millis();
    }

    const bool isValid() const { return millis() - lastSetTimeMs < expirationTimeMs; }

    const String getValue() const {
        if (isValid()) {
            return String(value, decimals);
        } else {
            return "";
        }
    }

    const float getValueFloat() const { return value; };

    const String toString() const {
        if (isValid()) {
            return "# HELP " + name + " " + help + "\n# TYPE " + name + " " + MetricTypeNames[type] + "\n" + name +
                   " " + getValue() + "\n";
        } else {
            return "";
        }
    };

   private:
    const String name;
    const String help;
    float value;
    const MetricType type;
    const unsigned int expirationTimeMs;
    unsigned long lastSetTimeMs = 0;
    unsigned int decimals = 1;
};

template <int MetricCount>
class Prometheus {
   public:
    Prometheus(const Metric* _metrics[]) : metrics(_metrics){};

    const String collect() const {
        String str;
        for (int i = 0; i < MetricCount; i++) {
            str += metrics[i]->toString();
        }
        return str;
    };

   private:
    const Metric** metrics;
};