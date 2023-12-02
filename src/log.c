#include "log.h"

// Modified from ChatGPT
void log_error(const char* _FILE_, const int _LINE_, const char* format, ...) {
    va_list args;
    va_start(args, format);

    // Print file name and line number
    printf("%s:%d: ", _FILE_, _LINE_);

    // Print the formatted string
    vprintf(format, args);

    va_end(args);
}