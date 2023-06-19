// ArduinoNvs.h

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

#pragma once

#include <Arduino.h>

extern "C" {
#include "esp_err.h"
#include "esp_partition.h"
#include "nvs.h"
#include "nvs_flash.h"
}

class ArduinoNvs {
   public:
    ArduinoNvs();

    bool begin(const char* namespaceNvs = "storage");
    void close();

    bool eraseAll(bool forceCommit = true);
    bool erase(String key, bool forceCommit = true);

    bool setInt(const char* key, uint8_t value, bool forceCommit = false);
    bool setInt(const char* key, int16_t value, bool forceCommit = false);
    bool setInt(const char* key, uint16_t value, bool forceCommit = false);
    bool setInt(const char* key, int32_t value, bool forceCommit = false);
    bool setInt(const char* key, uint32_t value, bool forceCommit = false);
    bool setInt(const char* key, int64_t value, bool forceCommit = false);
    bool setInt(const char* key, uint64_t value, bool forceCommit = false);
    bool setFloat(const char* key, float value, bool forceCommit = false);
    bool setString(const char* key, String value, bool forceCommit = false);
    bool setBlob(const char* key, void* blob, size_t length, bool forceCommit = false);

    bool setValue(const char* key, uint8_t value, bool forceCommit = false) { return setInt(key, value, forceCommit); };
    bool setValue(const char* key, bool value, bool forceCommit = false) { return setInt(key, (uint8_t)value, forceCommit); };
    bool setValue(const char* key, int16_t value, bool forceCommit = false) { return setInt(key, value, forceCommit); };
    bool setValue(const char* key, uint16_t value, bool forceCommit = false) {
        return setInt(key, value, forceCommit);
    };
    bool setValue(const char* key, int32_t value, bool forceCommit = false) { return setInt(key, value, forceCommit); };
    bool setValue(const char* key, uint32_t value, bool forceCommit = false) {
        return setInt(key, value, forceCommit);
    };
    bool setValue(const char* key, int64_t value, bool forceCommit = false) { return setInt(key, value, forceCommit); };
    bool setValue(const char* key, uint64_t value, bool forceCommit = false) {
        return setInt(key, value, forceCommit);
    };
    bool setValue(const char* key, float value, bool forceCommit = false) { return setFloat(key, value, forceCommit); };
    bool setValue(const char* key, String value, bool forceCommit = false) {
        return setString(key, value, forceCommit);
    };

    int64_t getInt(const char* key, int64_t default_value = 0);  // In case of error, default_value will be returned
    float getFloat(String key, float default_value = 0);

    bool getString(String key, String& res);
    String getString(String key);

    size_t getBlobSize(String key);  /// Returns the size of the stored blob
    bool getBlob(String key, void* blob,
                 size_t length);  /// User should proivde enought memory to store the loaded blob. If length < than
                                  /// required size to store blob, function fails.

    bool commit();

   protected:
    nvs_handle _nvs_handle = 0;
    String nvsNamespace;
};