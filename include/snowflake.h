#ifndef SNOWFLAKE_H
#define SNOWFLAKE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

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

#endif  // SNOWFLAKE_H
