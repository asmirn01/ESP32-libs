#include "WiFiManagerParameterExt.h"
#include "SysLogWrapper.h"

const char *WiFiManagerParameterExt::FALSE_STRING = "false";
const char *WiFiManagerParameterExt::TRUE_STRING = "true";
const int WiFiManagerParameterExt::BOOL_MAXLENGTH = 5;

bool WiFiManagerParameterExt::getValueAsInt(int &intValue) const {
	intValue = 0;
	const char *strValue = getValue();
	if (strValue == NULL) {
		return false;
	}
	if (strcmp(strValue, "0") != 0) {
		intValue = atoi(strValue);
		if (intValue == 0) {
			return false;
		}
	}
	return true;
}

void WiFiManagerParameterExt::setValue(const String &str) {
	WiFiManagerParameter::setValue(str.c_str(), maxLength);
	Log.deb << "Saving parameter " << getID() << " as '" << str << "'" << endl;
}
void WiFiManagerParameterExt::setValue(bool value) {
	WiFiManagerParameter::setValue(boolToString(value), maxLength);
	Log.deb << "Saving parameter " << getID() << " as '" << value << "'" << endl;
}
void WiFiManagerParameterExt::setValue(const char *str) {
	WiFiManagerParameter::setValue(str, maxLength);
	Log.deb << "Saving parameter " << getID() << " as '" << str << "'" << endl;
}