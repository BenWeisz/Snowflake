#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>

void log_error(const char* _FILE_, const int _LINE_, const char* format, ...);

#define LOG_ERROR(...) \
    log_error(__FILE__, __LINE__, __VA_ARGS__);

#endif  // LOG_H