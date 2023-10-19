// Bence Weisz - October 2023
// Putting everything in this file makes a mess but at least it'll all be in one file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***********************
    Utility Functions
***********************/

unsigned long hash(char* str);
int str_endswith(const char* str, const char* ending);
int strn_is_capital(const char* str, const int n);
int get_line_end_pos(char* str);
int is_snowflake_line(char* str);
int is_snowflake_attrib_line(char* str, const int indent);

unsigned long hash(char* str) {
    // Credits: https://stackoverflow.com/questions/7666509/hash-function-for-string
    unsigned long hash = 5381;
    
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

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

/*************************
    Snowflake file parser
*************************/

#define SNOWFLAKE_MAX_ATTRIB_NAME_LEN 128

#define SNOWFLAKE_TYPE_I32 0x00
#define SNOWFLAKE_TYPE_I64 0x01
#define SNOWFLAKE_TYPE_U32 0x02
#define SNOWFLAKE_TYPE_U64 0x03

#define SNOWFLAKE_TYPE_F32 0x04
#define SNOWFLAKE_TYPE_F64 0x05

#define SNOWFLAKE_TYPE_STR 0x06

#define SNOWFLAKE_TYPE_ARRAY 0x07

typedef struct SNOWFLAKE {
    void** fields;
    int num_fields;
    int is_base_type;
} SNOWFLAKE;

typedef struct SNOWFLAKE_I32 {
    int type;
    int default_value;
    char name[SNOWFLAKE_MAX_ATTRIB_NAME_LEN];
} SNOWFLAKE_I32;

typedef struct SNOWFLAKE_I64 {
    int type;
    long default_value;
    char name[SNOWFLAKE_MAX_ATTRIB_NAME_LEN];
} SNOWFLAKE_I64;

typedef struct SNOWFLAKE_U32 {
    int type;
    unsigned int default_value;
    char name[SNOWFLAKE_MAX_ATTRIB_NAME_LEN];
} SNOWFLAKE_U32;

typedef struct SNOWFLAKE_U64 {
    int type;
    unsigned long default_value;
    char name[SNOWFLAKE_MAX_ATTRIB_NAME_LEN];
} SNOWFLAKE_U64;

typedef struct SNOWFLAKE_F32 {
    int type;
    float default_value;
    char name[SNOWFLAKE_MAX_ATTRIB_NAME_LEN];
} SNOWFLAKE_F32;

typedef struct SNOWFLAKE_F64 {
    int type;
    double default_value;
    char name[SNOWFLAKE_MAX_ATTRIB_NAME_LEN];
} SNOWFLAKE_F64;

typedef struct SNOWFLAKE_STR {
    int type;
    int max_length;
    char name[SNOWFLAKE_MAX_ATTRIB_NAME_LEN];
} SNOWFLAKE_STR;

typedef struct SNOWFLAKE_ARRAY {
    int type;
    SNOWFLAKE* template;
} SNOWFLAKE_ARRAY;

typedef struct SNOWFLAKES {
    SNOWFLAKE* snowflakes;
    int num_snowflakes;
} SNOWFLAKES;

void init_snowflake(SNOWFLAKE* snowflake);
void load_snowflakes(const char* filename, SNOWFLAKES* snowflakes);

void init_snowflake(SNOWFLAKE* snowflake) {
    snowflake->fields = NULL;
    snowflake->num_fields = 0;
    snowflake->is_base_type = 0;
}

void load_snowflakes(const char* filename, SNOWFLAKES* snowflakes) {
    if (str_endswith(filename, ".sf") != 1) {
        printf("ERROR: The path provided must be for a snowflake file (extension .sf)\n");
        return;
    }

    // Open the snowflake file
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("ERROR: Failed to open file %s\n", filename);
        return;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the data
    char* buffer = (char*)malloc(sizeof(char) * file_size);
    if (buffer == NULL) {
        printf("ERROR: Failed to allocate space for file buffer\n");
        fclose(file);
        return;
    }

    fread(buffer, sizeof(char), file_size, file);

    // The character position
    int curr_pos = 0;
    int line_end_pos = 0;

    // First Pass - Get the number of snowflakes
    snowflakes->num_snowflakes = 0;

    // Loop over lines, if lines starts contains only capitalized word or capitalized word followed wrapped in * then that is a snowflake
    while (curr_pos < file_size) {
        line_end_pos = get_line_end_pos(buffer + curr_pos);
        
        if (is_snowflake_line(buffer + curr_pos) == 1) {
            snowflakes->num_snowflakes++;
        }

        curr_pos += line_end_pos + 1;
    }

    // Allocate space for the snowflakes
    snowflakes->snowflakes = (SNOWFLAKE*)malloc(sizeof(SNOWFLAKE) * snowflakes->num_snowflakes);
    if (snowflakes->snowflakes == NULL) {
        printf("ERROR: Failed to allocate space for snowflakes\n");
        fclose(file);
        return;
    }

    // Initialize the snowflakes
    for (int snowflake_i = 0; snowflake_i < snowflakes->num_snowflakes; snowflake_i++) {
        init_snowflake(&(snowflakes->snowflakes[snowflake_i]));
    }

    // Get the amount of space need for the fields of each of the snowflakes
    curr_pos = 0;
    int snowflake_i = -1;
    while (curr_pos < file_size) {
        line_end_pos = get_line_end_pos(buffer + curr_pos);
        
        if (is_snowflake_line(buffer + curr_pos) == 1) {
            snowflake_i++;
            // Initialize base type snowflakes properly
            if ((buffer + curr_pos)[0] == '*') {
                snowflakes->snowflakes[snowflake_i].is_base_type = 1;
            }
        } else if (is_snowflake_attrib_line(buffer + curr_pos, 1) == 1) {
            snowflakes->snowflakes[snowflake_i].num_fields++;
        }

        curr_pos += line_end_pos + 1;
    }

    // Allocate space for each of the snowflakes' field pointers
    for (int snowflake_i = 0; snowflake_i < snowflakes->num_snowflakes; snowflake_i++) {
        snowflakes->snowflakes[snowflake_i].fields = (void**)malloc(sizeof(void*) * snowflakes->snowflakes[snowflake_i].num_fields);
    }

    // Cleanup
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("ERROR: Please provide exactly one parameter: A snowflake file with extension .sf\n");
        return 1;
    }

    SNOWFLAKES snowflakes;
    load_snowflakes(argv[1], &snowflakes);
}