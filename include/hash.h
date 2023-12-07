#ifndef HASH_H
#define HASH_H

#include <stdlib.h>
#include <string.h>

#include "params.h"
#include "log.h"

unsigned long long _sf_hash(const char* str, unsigned long long mod);

typedef struct _SF_HASHSET_ELEMENT {
    char* value;
    struct _SF_HASHSET_ELEMENT* next;
} _SF_HASHSET_ELEMENT;

typedef struct _SF_HASHSET {
    unsigned long long mod;
    _SF_HASHSET_ELEMENT** elements;
} _SF_HASHSET;

_SF_HASHSET* _sf_hashset_init(unsigned long long mod);
void _sf_hashset_free(_SF_HASHSET* sf_hashset);

int _sf_hashset_add_value(_SF_HASHSET* sf_hashset, const char* value);
int _sf_hashset_has_value(_SF_HASHSET* sf_hashset, const char* value);

#endif  // HASH_H