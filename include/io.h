#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>

#include "log.h"

typedef struct SF_FILE {
    char* data;
    unsigned int size;
} SF_FILE;

SF_FILE* sf_read_file(const char* path);

#endif  // IO_H