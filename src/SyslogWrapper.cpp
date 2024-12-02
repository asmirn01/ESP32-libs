#include "SyslogWrapper.h"

#include <Arduino.h>

#include "WiFiParameterManager.h"

const char *SyslogWrapper::logLevelNames[] = {[DEBUG] = "debug", [INFO] = "info", [ERROR] = "error"};

SyslogWrapper::SyslogWrapper(const char *deviceHostname, const char *appName)
	: _syslog(_udp, NULL, SYSLOG_PORT, deviceHostname, appName, LOG_USER, SYSLOG_PROTO_BSD),
	  wifiManagerParameterLoggingLevel("LoggingLevel", "Logging level (debug, info, error)", logLevelNames[DEBUG], 5) {};

size_t SyslogWrapper::send() {
	if (_buffer.isEmpty()) {
		return false;
	}

	size_t res = 0;
	if (WiFi.status() == WL_CONNECTED) {
		_buffer.trim();
		res = _syslog.write((const uint8_t *)_buffer.c_str(), _buffer.length());
	}
	_buffer.clear();
	return res;
}

size_t SyslogWrapper::write(uint8_t character) { return write(&character, 1); }

size_t SyslogWrapper::write(const uint8_t *data, size_t size) {
	Serial.write(data, size);
	if (_enabled) {
		_buffer.concat(data, size);
		if (_buffer.indexOf('\n') != -1) {
			unsigned int length = _buffer.length();
			if (send()) {
				return length;
			} else {
				return 0;
			};
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

bool SyslogWrapper::setLevel(const String &newLevel) {
	for (int i = 0; i < LogLevel::MAX_VALUE; i++) {
		if (newLevel.equalsIgnoreCase(logLevelNames[i])) {
			setLevel((LogLevel)i);
			return true;
		}
	}
	return false;
}

void SyslogWrapper::addLoggingLevelParameterToManager(WiFiParameterManager *manager) {
	manager->addParameter(&wifiManagerParameterLoggingLevel);
}

bool SyslogWrapper::validateLogLevelWMParameter() {
	bool ret = setLevel(wifiManagerParameterLoggingLevel.getValueAsString());
	if (!ret) {
		wifiManagerParameterLoggingLevel.setValue(logLevelNames[DEBUG]);
	}
	return ret;
};

size_t SyslogWrapperProxy::write(uint8_t byte) {
	if (level >= Log.getLevel()) {
		return Log.write(byte);
	}
	return 0;
};
