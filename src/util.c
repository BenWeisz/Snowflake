#include "../include/util.h"

unsigned long hash(char* str) {
    // Credits: https://stackoverflow.com/questions/7666509/hash-function-for-string
    unsigned long hash = 5381;
    
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

/* String functions */
int str_endswith(const char* str, const char* ending) {
    // Assumes str and ending are null-terminated
    int str_len = strlen(str);
    int ending_len = strlen(ending);

    if (str_len < ending_len || str_len == 0 || ending_len == 0)
        return 0;

    for (int i = 0; i < ending_len; i++) {
        if (str[str_len - 1 - i] != ending[ending_len - 1 - i])
            return 0;
    }

    return 1;
}

int strn_is_capital(const char* str, const int n) {
    if (n == 0)
        return 0;

    // Assumes str is null-terminated
    int str_len = strlen(str);
    for (int i = 0; i < str_len && i < n; i++) {
        if ((str[i] < 'A' || str[i] > 'Z') && str[i] != '_')
            return 0;
    }

    return 1;
}

/* .sf file specific helpers */
int get_line_end_pos(char* str) {
    // Assumes that str is null-terminated
    int pos = 0;
    while (str[pos] != '\n' && str[pos] != '\0') {
        pos++;
    }

    return pos;
}


int is_snowflake_line(char* str) {
    int line_end_pos = get_line_end_pos(str);

    return (strn_is_capital(str, line_end_pos) == 1) ||
           ((str[0] == '*') && (strn_is_capital(str + 1, line_end_pos - 2) == 1) && str[line_end_pos - 1] == '*');
}

int is_snowflake_attrib_line(char* str, const int indents) {
    // Asumes that str is null-terminated
    int line_end_pos = get_line_end_pos(str);

    int has_proper_indent = 1;
    int curr_indents = 0;
    int local_space_count = 0;
    for (int i = 0; i < line_end_pos; i++) {
        if (str[i] == ' ') {
            local_space_count++;
            if (local_space_count == 4) {
                local_space_count = 0;
                curr_indents++;
            }
        } else if (str[i] == '\t') {
            local_space_count = 0;
            curr_indents++;
        } else {
            break;
        }
    }

    return curr_indents == indents;
}