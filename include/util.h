#ifndef UTIL_H
#define UTIL_H

#include <string.h>

unsigned long hash(char* str);

/* String functions */
int str_endswith(const char* str, const char* ending);
int strn_is_capital(const char* str, const int n);

/* .sf file specific helpers */
int get_line_end_pos(char* str);
int is_snowflake_line(char* str);
int is_snowflake_attrib_line(char* str, const int indent);

#endif  // UTIL_H
