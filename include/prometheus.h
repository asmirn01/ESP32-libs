#pragma once

#include <WString.h>
#include <vector>

#define stringify(name) #name
enum MetricType { gauge, counter };
const char *MetricTypeNames[] = {[gauge] = stringify(gauge), [counter] = stringify(counter)};

class IMetric {
public:
	IMetric(const String &_name, const String &_help, const MetricType _type,
			const unsigned int _expirationTimeMs = 5000, const unsigned long _decimals = 1)
		: name(_name), help(_help), type(_type), expirationTimeMs(_expirationTimeMs), decimals(_decimals) {};

	virtual const String toString() const = 0;
	const bool isValid() const { return millis() - lastSetTimeMs < expirationTimeMs; }

protected:
	const String name;
	const String help;
	const MetricType type;
	const unsigned long expirationTimeMs;
	unsigned long lastSetTimeMs = 0;
	unsigned int decimals = 1;
};

class Metric : public IMetric {
public:
	Metric(const String &_name, const String &_help, const MetricType _type,
		   const unsigned int _expirationTimeMs = 5000, const unsigned long _decimals = 1)
		: IMetric(_name, _help, _type, _expirationTimeMs, _decimals) {};

	void setValue(const int newValue) { setValue((float)newValue); }
	void setValue(const long newValue) { setValue((float)newValue); }
	void setValue(const uint32_t newValue) { setValue((float)newValue); }
	void setValue(const float newValue) {
		value = newValue;
		lastSetTimeMs = millis();
	}

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
	float value;
};

class EnumTypeMetric : public IMetric {
public:
	EnumTypeMetric(const std::initializer_list<const char *> _states, const String &_name, const String &_help,
				   const MetricType _type, const unsigned int _expirationTimeMs = 5000,
				   const unsigned long _decimals = 1)
		: IMetric(_name, _help, _type, _expirationTimeMs, _decimals), states(_states) {};

	void setState(const char *state) {
		currentState = -1;
		for (int i = 0; i < states.size(); i++) {
			if (strcmp(states[i], state) == 0) {
				currentState = i;
				break;
			}
		}
        lastSetTimeMs = millis();
	}

	const String toString() const {
		String str;
		if (isValid()) {
			str += "# HELP " + name + " " + help + "\n# TYPE " + name + " " + MetricTypeNames[type] + "\n";
			for (int i = 0; i < states.size(); i++) {
				str += name + "{state=\"" + states[i] + "\"} ";
				str += (currentState == i) ? "1" : "0";
                str += "\n";
			}
		}
		return str;
	};

private:
	std::vector<const char *> states;
	int currentState = -1;
};

template <int MetricCount> class Prometheus {
public:
	Prometheus(const IMetric *_metrics[]) : metrics(_metrics) {};

	const String collect() const {
		String str;
		for (int i = 0; i < MetricCount; i++) {
			str += metrics[i]->toString();
		}
		return str;
	};

private:
	const IMetric **metrics;
};