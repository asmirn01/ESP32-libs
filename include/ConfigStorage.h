#pragma once

#include "Arduino.h"
#include "ArduinoJson.h"
#include "SPIFFS.h"
#include "SysLoggerWrapper.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "ConfigStorage"

#define FileFS SPIFFS

const char *FALSE_STRING = "false";
const char *TRUE_STRING = "true";
const int BOOL_MAXLENGTH = 5;

const char *boolToString(bool b) { return (b ? TRUE_STRING : FALSE_STRING); }

class ConfigStorage {
public:
	ConfigStorage(const char *path) : configFileName(path) {
		if (!FileFS.begin()) {
			ESP_LOGD(TAG, "FS failed to iniialize");
		} else {
			ESP_LOGD(TAG, "FS started");
		}
	};

	JsonDocument &get() { return configDoc; };

	void set(const JsonDocument &Document) { configDoc = Document; };

	void initialize() { configDoc.clear(); };

	bool revert() {
		initialize();
		return readConfigFile();
	};

	bool save() {
		File configFile = FileFS.open(configFileName, "w");

		if (configFile) {
			serializeJson(configDoc, configFile);
			configFile.close();

			ESP_LOGD(TAG, "Saved JSON config:");
			serializeJsonPretty(configDoc, Serial);
			return true;
		} else {
			ESP_LOGD(TAG, "Saving config file failed");
			return false;
		}
	};

	bool remove() {
		if (FileFS.exists(configFileName)) {
			ESP_LOGD(TAG, "Config file exists");

			if (FileFS.remove(configFileName)) {
				ESP_LOGD(TAG, "Removing config file OK");
				return true;
			} else {
				ESP_LOGD(TAG, "Removing config file failed");
				return false;
			}
		} else {
			ESP_LOGD(TAG, "Config file missing");
			return false;
		}
	};

	bool readConfigFile() {
		if (FileFS.exists(configFileName)) {
			ESP_LOGD(TAG, "Config file exists");

			File configFile = FileFS.open(configFileName, "r");
			if (!configFile) {
				ESP_LOGD(TAG, "Reading config file failed");
				return false;
			} else {
				deserializeJson(configDoc, configFile);
				ESP_LOGD(TAG, "Read JSON config:");
				serializeJsonPretty(configDoc, Serial);
				configFile.close();
				return true;
			}
		} else {
			ESP_LOGD(TAG, "Config file missing");
			return false;
		}
		return true;
	};

private:
	JsonDocument configDoc;
	const char *configFileName;
};