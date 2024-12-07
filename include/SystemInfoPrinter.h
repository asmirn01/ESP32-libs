#pragma once

#include <Arduino.h>

#include "SyslogWrapper.h"

class SystemInfoPrinter {

private:
	static const char *getFlashMode() {
		FlashMode_t ideMode = ESP.getFlashChipMode();
		return (ideMode == FM_QIO)	  ? "QIO"
			   : (ideMode == FM_QOUT) ? "QOUT"
			   : (ideMode == FM_DIO)  ? "DIO"
			   : (ideMode == FM_DOUT) ? "DOUT"
									  : "UNKNOWN";
	}

public:
	static void print(Print &out) {
		out << "Device " << ESP.getChipModel() << "; Flash size:" << ESP.getFlashChipSize()
			<< " bytes; Free sketch space: " << ESP.getFreeSketchSpace()
			<< " bytes; Flash speed: " << ESP.getFlashChipSpeed() / 1000000 << "MHz; Flash mode: " << getFlashMode()
			<< "; ESP SDK " << ESP.getSdkVersion() << "; Totalal PSRAM: " << ESP.getPsramSize() << "; Free PSRAM "
			<< ESP.getFreePsram() << endl;
	}
};
