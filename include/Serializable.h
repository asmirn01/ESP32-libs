#pragma once

#include "ArduinoNvs.h"
#ifdef TAG
#undef TAG
#endif
#define TAG "PreferencesManager"

class Serializable {
   public:
    Serializable(const char* key) : _key(key){};

    virtual void storeState() = 0;
    virtual void retrieveState() = 0;

    void initNVS() { nvs.begin(_key); }
    ArduinoNvs nvs;

   private:
    const char* _key;
};

template <int preferencesNumber>
class PreferencesManager {
   public:
    PreferencesManager(Serializable* serializables[]) : _serializables(serializables){};
    void storeState() {
        ESP_LOGD(TAG, "Storing preferences state");
        for (int i = 0; i < preferencesNumber; i++) {
            _serializables[i]->storeState();
        }
    }
    void retrieveState() {
        ESP_LOGD(TAG, "Retrieving preferences state");
        for (int i = 0; i < preferencesNumber; i++) {
            _serializables[i]->retrieveState();
        }
    }

    void begin() {
        ESP_LOGD(TAG, "PreferencesManager::begin");
        for (int i = 0; i < preferencesNumber; i++) {
            _serializables[i]->initNVS();
        }
    }

    Serializable** _serializables;
};