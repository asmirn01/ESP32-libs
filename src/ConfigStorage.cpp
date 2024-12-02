#include "ConfigStorage.h"
#include "SyslogWrapper.h"

ConfigStorage::ConfigStorage(const char *path)
	: configFileName(path) {
	if (!FileFS.begin()) {
		Log.err << "FS failed to iniialize" << endl;
	} else {
		Log.inf << "ConfigStorage. FS started" << endl;
	}
};

bool ConfigStorage::save() const{
	File configFile = FileFS.open(configFileName, "w");

	if (configFile) {
		serializeJson(configDoc, configFile);
		configFile.close();

		Log.inf << "Saved JSON config:";
		serializeJson(configDoc, Log.inf);
		Log.inf << endl;
		return true;
	} else {
		Log.err << "Saving config file failed" << endl;
		return false;
	}
};

bool ConfigStorage::remove() {
	configDoc.clear();
	if (FileFS.exists(configFileName)) {
		if (FileFS.remove(configFileName)) {
			Log.inf << "Removing config file OK" << endl;
			return true;
		} else {
			Log.err << "Removing config file failed" << endl;
			return false;
		}
	} else {
		Log.deb << "Config file missing" << endl;
		return false;
	}
};

bool ConfigStorage::readConfigFile() {
	if (FileFS.exists(configFileName)) {
		File configFile = FileFS.open(configFileName, "r");
		if (!configFile) {
			Log.err << "Reading config file failed" << endl;
			return false;
		} else {
			deserializeJson(configDoc, configFile);
			Log.deb << "Read JSON config:" << endl;
			serializeJsonPretty(configDoc, Serial);
			configFile.close();
			return true;
		}
	} else {
		Log.err << "Config file missing" << endl;
		return false;
	}
	return true;
};
