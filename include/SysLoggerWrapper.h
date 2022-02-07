#pragma once
#include <Syslog.h>

#ifndef syslog
extern Syslog syslog;
#endif

#undef ESP_LOGD
#define ESP_LOGD(tag, format, args...) \
if (WiFi.isConnected()) {\
    syslog.appName(tag); \
    syslog.logf(LOG_DEBUG, format, ##args);\
} else {\
    log_d(format, ##args);\
}

#undef ESP_LOGI
#define ESP_LOGI(tag, format, args...) \
if (WiFi.isConnected()) {\
    syslog.appName(tag); \
    syslog.logf(LOG_INFO, format, ##args);\
} else {\
    log_i(format, ##args);\
}

#undef ESP_LOGE
#define ESP_LOGE(tag, format, args...) \
if (WiFi.isConnected()) {\
    syslog.appName(tag); \
    syslog.logf(LOG_ERR, format, ##args);\
} else {\
    log_e(format, ##args);\
}
