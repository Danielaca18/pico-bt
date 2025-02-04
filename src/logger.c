#include "logger.h"

int logger(enum LOGGER_LEVEL log_level, const char* format, ...) {
    int bytes = 0;
    char time_tag[13], msg[LOGGER_MAX_LENGTH];

    log_time_t now = logger_time();
    logger_ms_to_str(now, time_tag);

    va_list args;
    va_start(args, format);
    vsnprintf(msg, LOGGER_MAX_LENGTH, format, args);
    va_end(args);

    #ifndef CMAKE_DEBUG
    if (log_level != DEBUG) bytes = fprintf(stdout, "%s [%s]: %s", time_tag, LOGGER_PREFIX[log_level], msg);
    #else
    bytes = fprintf(stdout, "%s [%s]: %s", time_tag, LOGGER_PREFIX[log_level], msg);
    #endif

    return bytes;
}

log_time_t logger_time() {
    uint64_t time_value = time_us_64() / 1000;

    log_time_t now;
    now.msec = time_value % 1000;
    time_value /= 1000;
    now.sec = time_value % 60;
    time_value /= 60;
    now.min = time_value % 60;
    now.hour = (time_value / 60) % 24;

    return now;
}

inline void logger_ms_to_str(log_time_t now, char* buffer) {
    sprintf(buffer, "%.2hhu:%.2hhu:%.2hhu:%.3hu", now.hour, now.min, now.sec, now.msec);
}