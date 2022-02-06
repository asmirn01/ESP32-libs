// ArduinoNvs.cpp

// Copyright (c) 2018 Sinai RnD
// Copyright (c) 2016-2017 TridentTD

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ArduinoNvs.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "ArduinoNvs"

#define RETURN_IF_NOT_INITIALIZED                       \
    if (!_nvs_handle) {                                 \
        ESP_LOGE(TAG, "NVS handle is not initialized"); \
        return false;                                   \
    }

ArduinoNvs::ArduinoNvs() {}

bool ArduinoNvs::begin(const char *namespaceNvs) {
    _nvs_handle = 0;
    nvsNamespace = namespaceNvs;
    ESP_LOGI(TAG, "Initializing NVS for %s", namespaceNvs);

    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Cannot init flash mem");
        if (err != ESP_ERR_NVS_NO_FREE_PAGES) {
            ESP_LOGE(TAG, "No free pages left in NVS");
            return false;
        }

        // erase and reinit
        ESP_LOGI(TAG, "Try reinit the partition");
        const esp_partition_t *nvs_partition =
            esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
        if (nvs_partition == NULL) {
            ESP_LOGI(TAG, "Can't find NVS partition");
            return false;
        }
        err = esp_partition_erase_range(nvs_partition, 0, nvs_partition->size);
        esp_err_t err = nvs_flash_init();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Cannot init flash mem after reformat");
            return false;
        }
        ESP_LOGI(TAG, "Partition re-formatted");
    }

    err = nvs_open(namespaceNvs, NVS_READWRITE, &_nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open failed");
        _nvs_handle = 0;
        return false;
    }
    ESP_LOGD(TAG, "NVS init succeeded");

    return true;
}

void ArduinoNvs::close() {
    if (_nvs_handle) {
        nvs_close(_nvs_handle);
    }
}

bool ArduinoNvs::eraseAll(bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    esp_err_t err = nvs_erase_all(_nvs_handle);
    if (err != ESP_OK) {
        return false;
    }
    return forceCommit ? commit() : true;
}

bool ArduinoNvs::erase(String key, bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    esp_err_t err = nvs_erase_key(_nvs_handle, key.c_str());
    if (err != ESP_OK) {
        return false;
    }
    return forceCommit ? commit() : true;
}

bool ArduinoNvs::commit() {
    RETURN_IF_NOT_INITIALIZED

    esp_err_t err = nvs_commit(_nvs_handle);
    if (err != ESP_OK) {
        return false;
    }
    return true;
}

bool ArduinoNvs::setInt(const char *key, uint8_t value, bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    ESP_LOGD(TAG, "NVS[%s]. SetInt(%s, %" PRIu8 ")", nvsNamespace.c_str(), key, value);
    esp_err_t err = nvs_set_u8(_nvs_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save int. err = [0x%X]", err);
        return false;
    }

    return forceCommit ? commit() : true;
}

bool ArduinoNvs::setInt(const char *key, int16_t value, bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    ESP_LOGD(TAG, "NVS[%s]. SetInt(%s, %" PRIi16 ")", nvsNamespace.c_str(), key, value);
    esp_err_t err = nvs_set_i16(_nvs_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save int. err = [0x%X]", err);
        return false;
    }
    return forceCommit ? commit() : true;
}

bool ArduinoNvs::setInt(const char *key, uint16_t value, bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    ESP_LOGD(TAG, "NVS[%s]. SetInt(%s, %" PRIu16 ")", nvsNamespace.c_str(), key, value);
    esp_err_t err = nvs_set_u16(_nvs_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save int. err = [0x%X]", err);
        return false;
    }

    return forceCommit ? commit() : true;
}

bool ArduinoNvs::setInt(const char *key, int32_t value, bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    ESP_LOGD(TAG, "NVS[%s]. SetInt(%s, %" PRIi32 ")", nvsNamespace.c_str(), key, value);
    esp_err_t err = nvs_set_i32(_nvs_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save int. err = [0x%X]", err);
        return false;
    }

    return forceCommit ? commit() : true;
}

bool ArduinoNvs::setInt(const char *key, uint32_t value, bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    ESP_LOGD(TAG, "NVS[%s]. SetInt(%s, %" PRIu32 ")", nvsNamespace.c_str(), key, value);
    esp_err_t err = nvs_set_u32(_nvs_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save int. err = [0x%X]", err);
        return false;
    }
    return forceCommit ? commit() : true;
}
bool ArduinoNvs::setInt(const char *key, int64_t value, bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    ESP_LOGD(TAG, "NVS[%s]. SetInt(%s, %" PRIi64 ")", nvsNamespace.c_str(), key, value);
    esp_err_t err = nvs_set_i64(_nvs_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save int. err = [0x%X]", err);
        return false;
    }
    return forceCommit ? commit() : true;
}

bool ArduinoNvs::setInt(const char *key, uint64_t value, bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    ESP_LOGD(TAG, "NVS[%s]. SetInt(%s, %" PRIu64 ")", nvsNamespace.c_str(), key, value);
    esp_err_t err = nvs_set_u64(_nvs_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save int. err = [0x%X]", err);
        return false;
    }
    return forceCommit ? commit() : true;
}

bool ArduinoNvs::setString(const char *key, String value, bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    ESP_LOGD(TAG, "NVS[%s]. setString(%s, %s)", nvsNamespace.c_str(), key, value.c_str());
    esp_err_t err = nvs_set_str(_nvs_handle, key, value.c_str());

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save String. err = [0x%X]", err);
        return false;
    }
    return forceCommit ? commit() : true;
}

bool ArduinoNvs::setBlob(const char *key, void *blob, size_t length, bool forceCommit) {
    RETURN_IF_NOT_INITIALIZED

    ESP_LOGD(TAG, "NVS[%s]. setBlob(%s, addr = [0x%X], length = [%d])", nvsNamespace.c_str(), key, (int32_t)blob,
             length);
    if (length == 0) {
        return false;
    }
    esp_err_t err = nvs_set_blob(_nvs_handle, key, blob, length);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save BLOB. err = [0x%X]", err);
        return false;
    }
    return forceCommit ? commit() : true;
}

int64_t ArduinoNvs::getInt(const char *key, int64_t default_value) {
    union {
        uint8_t v_u8;
        int16_t v_i16;
        uint16_t v_u16;
        int32_t v_i32;
        uint32_t v_u32;
        int64_t v_i64;
        uint64_t v_u64;
    };

    esp_err_t err;

    RETURN_IF_NOT_INITIALIZED

    err = nvs_get_u8(_nvs_handle, key, &v_u8);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "NVS[%s]. Retrieved value %" PRIu8 " of property %s", nvsNamespace.c_str(), v_u8, key);
        return (int64_t)v_u8;
    }

    err = nvs_get_i16(_nvs_handle, key, &v_i16);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "NVS[%s]. Retrieved value %" PRIi16 " of property %s", nvsNamespace.c_str(), v_i16, key);
        return (int64_t)v_i16;
    }

    err = nvs_get_u16(_nvs_handle, key, &v_u16);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "NVS[%s]. Retrieved value %" PRIu16 " of property %s", nvsNamespace.c_str(), v_u16, key);
        return (int64_t)v_u16;
    }

    err = nvs_get_i32(_nvs_handle, key, &v_i32);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "NVS[%s]. Retrieved value %" PRIi32 " of property %s", nvsNamespace.c_str(), v_i32, key);
        return (int64_t)v_i32;
    }

    err = nvs_get_u32(_nvs_handle, key, &v_u32);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "NVS[%s]. Retrieved value %" PRIu32 " of property %s", nvsNamespace.c_str(), v_u32, key);
        return (int64_t)v_u32;
    }

    err = nvs_get_i64(_nvs_handle, key, &v_i64);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "NVS[%s]. Retrieved value %" PRIi64 " of property %s", nvsNamespace.c_str(), v_i64, key);
        return (int64_t)v_i64;
    }

    err = nvs_get_u64(_nvs_handle, key, &v_u64);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "NVS[%s]. Retrieved value %" PRIu64 " of property %s", nvsNamespace.c_str(), v_u64, key);
        return (int64_t)v_u64;
    }
    ESP_LOGE(TAG, "Unable to find value of property %s", key);

    return default_value;
}

bool ArduinoNvs::getString(String key, String &res) {
    size_t required_size;
    esp_err_t err;

    RETURN_IF_NOT_INITIALIZED

    err = nvs_get_str(_nvs_handle, key.c_str(), NULL, &required_size);
    if (err != ESP_OK) {
        return false;
    }

    char value[required_size];
    err = nvs_get_str(_nvs_handle, key.c_str(), value, &required_size);
    if (err != ESP_OK) {
        return false;
    }
    res = value;
    return true;
}

String ArduinoNvs::getString(String key) {
    String res;

    bool ok = getString(key, res);
    if (!ok) {
        return emptyString;
    }
    return res;
}

size_t ArduinoNvs::getBlobSize(String key) {
    RETURN_IF_NOT_INITIALIZED

    size_t required_size;
    esp_err_t err = nvs_get_blob(_nvs_handle, key.c_str(), NULL, &required_size);
    if (err) {
        if (err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "Error getting key %s is not found from the store. err = [0x%X]", key.c_str(), err);
        }
        return 0;
    }
    return required_size;
}

bool ArduinoNvs::getBlob(String key, void *blob, size_t length) {
    RETURN_IF_NOT_INITIALIZED
    if (length == 0) {
        return false;
    }

    size_t required_size = getBlobSize(key);
    if (required_size == 0) {
        return false;
    }
    if (length < required_size) {
        return false;
    }

    esp_err_t err = nvs_get_blob(_nvs_handle, key.c_str(), blob, &required_size);
    if (err) {
        ESP_LOGE(TAG, "Get blob err = [0x%X]", err);
        return false;
    }
    return true;
}

bool ArduinoNvs::setFloat(const char *key, float value, bool forceCommit) {
    return setBlob(key, (uint8_t *)&value, sizeof(float), forceCommit);
}

float ArduinoNvs::getFloat(String key, float default_value) {
    RETURN_IF_NOT_INITIALIZED

    float data;
    if (!getBlob(key, (uint8_t *)&data, sizeof(float))) {
        return default_value;
    }
    return data;
}
