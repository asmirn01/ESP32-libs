#pragma once

#include <Syslog.h>
#include <WiFi.h>

#include "WiFiManagerParameterExt.h"

#define SYSLOG_PORT 514

enum LogLevel { DEBUG, INFO, ERROR, MAX_VALUE };

class SyslogWrapperProxy : public Print {
public:
	SyslogWrapperProxy(LogLevel _level) : level(_level) {};
	virtual size_t write(uint8_t byte);

private:
	const LogLevel level;
};

class WiFiParameterManager;

class SyslogWrapper : public Print {
public:
	SyslogWrapper(const char *deviceHostname, const char *appName);

	virtual size_t write(uint8_t character);
	virtual size_t write(const uint8_t *data, size_t size);
	void setSyslogHost(const char *host) { _syslog.server(host, SYSLOG_PORT); }
	void setEnabled(boolean enabled) {
		_enabled = enabled;
		if (!enabled) {
			_buffer.clear();
		}
	};
	bool isEnabled() { return _enabled; };

	void setLevel(LogLevel level) { enabledLevel = level; };
	bool setLevel(const String &level);
	LogLevel getLevel() const { return enabledLevel; };
	const char *getLevelAsString() const { return logLevelNames[enabledLevel]; };

	SyslogWrapperProxy deb = SyslogWrapperProxy(LogLevel::DEBUG);
	SyslogWrapperProxy inf = SyslogWrapperProxy(LogLevel::INFO);
	SyslogWrapperProxy err = SyslogWrapperProxy(LogLevel::ERROR);

	bool validateLogLevelWMParameter();
	void addLoggingLevelParameterToManager(WiFiParameterManager *manager);

	static const char *logLevelNames[];

private:
	size_t send();

	WiFiUDP _udp;
	Syslog _syslog;
	String _buffer;
	bool _enabled = true;
	LogLevel enabledLevel = LogLevel::DEBUG;
	WiFiManagerParameterExt wifiManagerParameterLoggingLevel;
};

extern SyslogWrapper Log;

template <class T> inline Print &operator<<(Print &stream, T arg) {
	stream.print(arg);
	return stream;
}

enum _EndLineCode { endl };

inline Print &operator<<(Print &stream, _EndLineCode arg) {
	stream.println();
	return stream;
}
