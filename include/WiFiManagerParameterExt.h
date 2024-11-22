#pragma once

#include <Arduino.h>

#include "ConfigStorage.h"
#include "SysLoggerWrapper.h"
#include "WiFiManager.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "WiFiManagerParameterExt"

class WiFiManagerParameterExt : public WiFiManagerParameter {
public:
	WiFiManagerParameterExt(const char *id, const char *label, const char *defaultValue, int _maxLength)
		: WiFiManagerParameter(id, label, defaultValue, _maxLength), maxLength(_maxLength) {};

	void setValue(const String &str) {
		WiFiManagerParameter::setValue(str.c_str(), maxLength);
		ESP_LOGD(TAG, "Saving parameter '%s' as '%s'", getID(), str.c_str());
	}
	void setValue(bool value){
		WiFiManagerParameter::setValue(boolToString(value), maxLength);
		ESP_LOGD(TAG, "Saving parameter '%s' as '%s'", getID(), boolToString(value));
	}
	void setValue(const char* str) {
		WiFiManagerParameter::setValue(str, maxLength);
		ESP_LOGD(TAG, "Saving parameter '%s' as '%s'", getID(), str);
	}
	String getValueAsString() const { return (String)WiFiManagerParameter::getValue(); }
	bool getValueAsBool() const { return getValueAsString().equalsIgnoreCase("true") ? true : false; }

	const int maxLength;
};

template <int ParameterCount> class WiFiParameterManager {
public:
	WiFiParameterManager(WiFiManagerParameterExt *_parameters[], ConfigStorage &_configStorage)
		: parameters(_parameters), configStorage(_configStorage) {}

	void saveAllParameters() const {
		for (int i = 0; i < ParameterCount; i++) {
			configStorage.get()[parameters[i]->getID()] = parameters[i]->getValue();
		}
		configStorage.save();
	}

	void addAllToWiFiManager(WiFiManager& wifiManager) {
		for (int i = 0; i < ParameterCount; i++) {
			wifiManager.addParameter(parameters[i]);
		}
	}

	bool loadAllParameters() {
		if (configStorage.readConfigFile()) {
			for (int i = 0; i < ParameterCount; i++) {
				parameters[i]->setValue(configStorage.get()[parameters[i]->getID()].as<const char*>());
			}
			return true;
		} else {
			ESP_LOGD(TAG, "Unable to read config file");
			return false;
		}
	}

private:
	WiFiManagerParameterExt **parameters;
	ConfigStorage &configStorage;
};