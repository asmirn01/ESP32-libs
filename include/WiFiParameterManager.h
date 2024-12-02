#pragma once

#include <vector>

#include "ConfigStorage.h"
#include "WiFiManagerParameterExt.h"

class WiFiParameterManager {
public:
	WiFiParameterManager(ConfigStorage &_configStorage, std::initializer_list<WiFiManagerParameterExt *> _parameters)
		: configStorage(_configStorage), parameters(_parameters) {}

	void addParameter(WiFiManagerParameterExt *parameter) { parameters.push_back(parameter); }

	void saveAllParameters() {
		for (WiFiManagerParameterExt *param : parameters) {
			configStorage.get()[param->getID()] = param->getValue();
		}
		configStorage.save();
	}

	void addAllToWiFiManager(WiFiManager &wifiManager) {
		for (WiFiManagerParameterExt *param : parameters) {
			wifiManager.addParameter(param);
		}
	}

	bool loadAllParameters() {
		if (configStorage.readConfigFile()) {
			for (WiFiManagerParameterExt *param : parameters) {
				param->setValue(configStorage.get()[param->getID()].as<const char *>());
			}
			return true;
		} else {
			Log.err << "Unable to read config file" << endl;
			return false;
		}
	}

private:
	ConfigStorage &configStorage;
	std::vector<WiFiManagerParameterExt *> parameters;
};