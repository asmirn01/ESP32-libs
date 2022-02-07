#include <Arduino.h>

#ifdef TAG
#undef TAG
#endif
#define TAG "CurrentSensor"

#define SINGLE_SAMPLE_DURATION 500  // sample for 25 full AC periods

// int mVperAmp = 185;           // 5A:185, 20A:100, 30A:66
class CurrentSensor {
   public:
    CurrentSensor(int _pin, int _threshold, int _index) : pin(_pin), threshold(_threshold), index(_index) {}

   public:
    int pin;
    int threshold;
    int index;
};

template <int SENSOR_COUNT>
class CurrentSensorManager {
   public:
    struct CurrentSensorData {
        uint8_t state;
        uint16_t deltas[SENSOR_COUNT];
    };

    CurrentSensorManager(CurrentSensor* _sensors[], int _sampleIntervalMs)
        : sensors(_sensors), sampleIntervalMs(_sampleIntervalMs){};

    bool getStatusOfAllSensors(CurrentSensorData& data) {
        if (measuredCurrentDeltaQueue == NULL || measureCurrentDeltaTaskHandle == NULL) {
            ESP_LOGE(TAG,
                     "Either measureCurrentDeltaTaskHandle or measuredCurrentDeltaQueue are not initialized. Most "
                     "likely CurrentSensorManager::begin() was not called");
            return false;
        }
        if (uxQueueMessagesWaiting(measuredCurrentDeltaQueue) != 0) {
            xQueueReceive(measuredCurrentDeltaQueue, &data, 1);
            return true;
        } else {
            return false;
        }
    }

    void begin() {
        if (SENSOR_COUNT > 8) {
            ESP_LOGE(TAG, "Too many sensors defined! Only 8 are allowed");
        }
        measuredCurrentDeltaQueue = xQueueCreate(1, sizeof(CurrentSensorData));
        if (measuredCurrentDeltaQueue == 0) {
            ESP_LOGE(TAG, "Failed to create data transfer queue for the current measurement task");
        }

        if (xTaskCreatePinnedToCore(currentMeasurementWorker, "Current Sampler Task", 2048, (void*)this, 5,
                                    &measureCurrentDeltaTaskHandle, 0) != pdPASS) {
            ESP_LOGE(TAG, "Failed to create current measurement task");
        }

        if (sampleIntervalMs <= SINGLE_SAMPLE_DURATION) {
            ESP_LOGE(TAG, "sampleIntervalMs must be greater than %d", SINGLE_SAMPLE_DURATION);
        }

        for (int i = 0; i < SENSOR_COUNT; i++) {
            pinMode(sensors[i]->pin, INPUT);
        }
    }

    static void currentMeasurementWorker(void* thisObj) {
        QueueHandle_t queue = ((CurrentSensorManager*)thisObj)->measuredCurrentDeltaQueue;
        CurrentSensor** sensors = ((CurrentSensorManager*)thisObj)->sensors;
        int sampleIntervalMs = ((CurrentSensorManager*)thisObj)->sampleIntervalMs;
        CurrentSensorData data;

        int maxValue[SENSOR_COUNT];
        int minValue[SENSOR_COUNT];

        while (1) {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                maxValue[i] = {0};
                minValue[i] = {4095};
            }
            uint32_t start_time = millis();
            while ((millis() - start_time) < SINGLE_SAMPLE_DURATION) {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    int readValue = analogRead(sensors[i]->pin);
                    if (readValue > maxValue[i]) {
                        maxValue[i] = readValue;
                    }
                    if (readValue < minValue[i]) {
                        minValue[i] = readValue;
                    }
                }
                delay(1);  // 20 samples per period
            }

            uint8_t mask = 0;
            for (int i = 0; i < SENSOR_COUNT; i++) {
                data.deltas[i] = maxValue[i] - minValue[i];
                bool isWorking = (data.deltas[i] >= sensors[i]->threshold);
                //                ESP_LOGD(TAG, "Status of sensor %d is %d (delta %d)", i, isWorking, delta);
                if (isWorking) {
                    bitSet(mask, i);
                }
            }

            data.state = mask;
            xQueueOverwrite(queue, &data);
            delay(sampleIntervalMs - (millis() - start_time));
        }
    }

    CurrentSensor** sensors;
    TaskHandle_t measureCurrentDeltaTaskHandle = NULL;
    QueueHandle_t measuredCurrentDeltaQueue = NULL;
    int sampleIntervalMs = 0;
};
