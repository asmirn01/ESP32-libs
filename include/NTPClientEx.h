#pragma once

#include "NTPClient.h"

class NTPClientEx : public NTPClient {
   public:
    NTPClientEx(UDP& udp) : NTPClient(udp){};
    NTPClientEx(UDP& udp, int timeOffset) : NTPClient(udp, timeOffset){};
    NTPClientEx(UDP& udp, const char* poolServerName) : NTPClient(udp, poolServerName){};
    NTPClientEx(UDP& udp, const char* poolServerName, int timeOffset) : NTPClient(udp, poolServerName, timeOffset){};
    NTPClientEx(UDP& udp, const char* poolServerName, int timeOffset, int updateInterval)
        : NTPClient(udp, poolServerName, timeOffset, updateInterval){};

    bool isSynchronized() { return getEpochTime() != 0; };
};