#pragma once

#include <Arduino.h>

class WrappableTimer {
   public:
    template <typename callback_t>
    WrappableTimer(unsigned long intervalMs, callback_t callback, bool bTriggerImmediately = false,
                   bool initiallyActive = true, void* userData = NULL)
        : m_intervalMs(intervalMs), m_callback(callback), m_active(initiallyActive), m_userData(userData) {
        if (bTriggerImmediately) {
            m_lastTriggeredTimeMs = millis() - intervalMs;
        } else {
            m_lastTriggeredTimeMs = millis();
        }
    }

    void deactivate() { m_active = false; }

    unsigned long elapsed() const {
        if (!m_active) {
            return 0;
        }
        unsigned long now = millis();
        unsigned long timeDelta;
        if (m_lastTriggeredTimeMs <= now) {
            timeDelta = now - m_lastTriggeredTimeMs;
        } else {
            timeDelta = ULONG_MAX - m_lastTriggeredTimeMs + now;
        }
        return timeDelta;
    }

    unsigned long timeUntlNextTriggerMs() { return m_intervalMs - elapsed(); }

    void elapsedStr(char* buffer, unsigned int bufferSize) const {
        if (!m_active || bufferSize < 6) {
            buffer[0] = '\0';
            return;
        }

        unsigned long elapsedSec = elapsed() / 1000;
        unsigned char minutes = elapsedSec / 60;
        unsigned char seconds = elapsedSec % 60;

        buffer[0] = minutes / 10 + '0';
        buffer[1] = minutes % 10 + '0';
        buffer[2] = ':';
        buffer[3] = seconds / 10 + '0';
        buffer[4] = seconds % 10 + '0';
        buffer[5] = '\0';
    }

    void process() {
        if (!m_active) {
            return;
        }

        if (elapsed() > m_intervalMs) {
            triggerAndReshedule();
        }
    }

    void processNoReset() {
        if (!m_active) {
            return;
        }

        if (elapsed() > m_intervalMs) {
            m_callback(m_userData);
        }
    }

    void reset() {
        m_lastTriggeredTimeMs = millis();
        m_active = true;
    }

    void triggerAndReshedule() {
        m_lastTriggeredTimeMs = millis();
        m_callback(m_userData);
    }

    bool isActive() { return m_active; };

   private:
    const unsigned long m_intervalMs;
    void (*m_callback)(void*);
    bool m_active;
    unsigned long m_lastTriggeredTimeMs;
    void* m_userData;
};

template <int TimerCount>
class WrappableTimerManager {
   public:
    WrappableTimerManager(WrappableTimer* _timers[]) : timers(_timers) {}
    void processAllTimers() {
        for (int i = 0; i < TimerCount; i++) {
            timers[i]->process();
        }
    }

    /** returns ULONG_MAX if no timers are active */
    unsigned long timeUntilNextTriggerMs() {
        unsigned long timeUntilNextTrigger = ULONG_MAX;
        for (int i = 0; i < TimerCount; i++) {
            if (timers[i]->isActive()) {
                timeUntilNextTrigger = min(timeUntilNextTrigger, timers[i]->timeUntlNextTriggerMs());
            }
        }
        return timeUntilNextTrigger;
    }

   private:
    WrappableTimer** timers;
};