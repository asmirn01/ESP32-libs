#pragma once

#include <Arduino.h>
#include <vector>

#include "WiFiManager.h"

class WiFiManagerParameterExt : public WiFiManagerParameter {
public:
	WiFiManagerParameterExt(const char *id, const char *label, const char *defaultValue, int _maxLength)
		: WiFiManagerParameter(id, label, defaultValue, _maxLength), maxLength(_maxLength) {};

	void setValue(const String &str);
	void setValue(bool value);
	void setValue(const char *str);
	String getValueAsString() const { return (String)WiFiManagerParameter::getValue(); };
	bool getValueAsBool() const { return getValueAsString().equalsIgnoreCase(TRUE_STRING) ? true : false; };
	bool getValueAsInt(int &value) const;

	static const char *FALSE_STRING;
	static const char *TRUE_STRING;
	static const int BOOL_MAXLENGTH;

private:
	static const char *boolToString(bool b) { return (b ? TRUE_STRING : FALSE_STRING); }

	const int maxLength;
};
