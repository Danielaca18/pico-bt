#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include "pico/time.h"
#include <stdarg.h>
#include <time.h>

#define LOGGER_MAX_LENGTH           256

typedef struct {
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t msec;
} log_time_t;

enum LOGGER_LEVEL {
    INFO = 0,
    DEBUG = 1,
    WARNING,
    ERROR
};

static const char* const LOGGER_PREFIX[] = {
    [INFO] = "INFO",
    [DEBUG] = "DEBUG",
    [WARNING] = "WARNING",
};

int logger(enum LOGGER_LEVEL log_level, const char* format, ...);
log_time_t logger_time();
void logger_ms_to_str(log_time_t now, char* buffer);


#endif