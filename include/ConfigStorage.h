#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#define FileFS SPIFFS

class ConfigStorage {
public:
	ConfigStorage(const char *path);

	JsonDocument &get() { return configDoc; };
	void set(const JsonDocument &Document) { configDoc = Document; };

	void initialize() { configDoc.clear(); };
	bool revert() {
		initialize();
		return readConfigFile();
	};

	bool save() const;
	bool remove();
	bool readConfigFile();

private:
	JsonDocument configDoc;
	const char *configFileName;
};