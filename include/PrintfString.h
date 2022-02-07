#pragma once

#include "WString.h"

class PrintfString : public String {
   public:
    void printf(const char *format, ...) {
        static char loc_buf[64];
        char *temp = loc_buf;
        int len;
        va_list arg;
        va_start(arg, format);
        len = vsnprintf(NULL, 0, format, arg);
        if (len >= sizeof(loc_buf)) {
            temp = (char *)malloc(len + 1);
            if (temp == NULL) {
                va_end(arg);
                return;
            }
        }
        va_end(arg);

        va_list arg1;
        va_start(arg1, format);
        vsnprintf(temp, len + 1, format, arg1);
        va_end(arg1);
        String::operator=((const char *)temp);
        if (len >= sizeof(loc_buf)) {
            free(temp);
        }
    }
};