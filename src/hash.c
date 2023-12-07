#include "hash.h"

unsigned long long _sf_hash(const char* str, unsigned long long mod) {
    unsigned long long hash = 5381;

    unsigned long long modv = mod;
    if (modv == 0) modv = 1099511628400;

    const char* curr_char = str;
    while (*curr_char != '\0') {
        hash = (hash << 5) + hash + ((long long)*curr_char);
        hash = hash % modv;
        curr_char++;
    }

    return hash;
}

_SF_HASHSET* _sf_hashset_init(unsigned long long mod) {
    // Allocate memory for hashset
    _SF_HASHSET* sf_hashset = (_SF_HASHSET*)malloc(sizeof(_SF_HASHSET));
    if (sf_hashset == NULL) {
        LOG_ERROR("ERROR: Failed to allocate memory for _SF_HASHSET\n");
        return NULL;
    }

    sf_hashset->mod = mod;

    // Allocate memory for elements
    sf_hashset->elements = (_SF_HASHSET_ELEMENT**)malloc(sizeof(_SF_HASHSET_ELEMENT*) * mod);
    if (sf_hashset->elements == NULL) {
        LOG_ERROR("ERROR: Failed to allocate memory for elements\n");
        free(sf_hashset);
        return NULL;
    }

    // Clear all the pointers
    memset(sf_hashset->elements, 0, sizeof(_SF_HASHSET_ELEMENT*) * mod);

    return sf_hashset;
}

void _sf_hashset_free(_SF_HASHSET* sf_hashset) {
    // Free the elements
    for (unsigned long long element_i = 0; element_i < sf_hashset->mod; element_i++) {
        _SF_HASHSET_ELEMENT* curr_element = sf_hashset->elements[element_i];
        _SF_HASHSET_ELEMENT* temp_element;
        while (curr_element != NULL) {
            free(curr_element->value);
            temp_element = curr_element->next;
            free(curr_element);
            curr_element = temp_element;
        }
    }

    // Free the rest of the structure
    free(sf_hashset->elements);
    free(sf_hashset);
}

int _sf_hashset_add_value(_SF_HASHSET* sf_hashset, const char* value) {
    unsigned long long hash = _sf_hash(value, sf_hashset->mod);

    _SF_HASHSET_ELEMENT** sf_hashset_element_ptr = &(sf_hashset->elements[hash]);
    while (*sf_hashset_element_ptr != NULL) {
        if (strcmp((*sf_hashset_element_ptr)->value, value) == 0)
            return 0;
        sf_hashset_element_ptr = &((*sf_hashset_element_ptr)->next);
    }

    // Allocate space for sf_hashset_element
    _SF_HASHSET_ELEMENT* new_sf_hashset_element = (_SF_HASHSET_ELEMENT*)malloc(sizeof(_SF_HASHSET_ELEMENT));
    if (new_sf_hashset_element == NULL) {
        LOG_ERROR("ERROR: Failed to allocate memory for _SF_HASHSET_ELEMENT\n");
        return 1;
    }
    new_sf_hashset_element->next = NULL;

    *sf_hashset_element_ptr = new_sf_hashset_element;

    // Allocate enough memory to store the value
    int value_len = strlen(value);
    new_sf_hashset_element->value = (char*)malloc(sizeof(char) * (value_len + 1));
    if (new_sf_hashset_element->value == NULL) {
        LOG_ERROR("ERROR: Failed to allocate memory for _SF_HASHSET_ELEMENT value\n");
        *sf_hashset_element_ptr = NULL;
        free(new_sf_hashset_element);
        return 1;
    }

    // Copy in the value
    strcpy(new_sf_hashset_element->value, value);
    new_sf_hashset_element->value[value_len] = '\0';

    return 0;
}

int _sf_hashset_has_value(_SF_HASHSET* sf_hashset, const char* value) {
    unsigned long long hash = _sf_hash(value, sf_hashset->mod);

    _SF_HASHSET_ELEMENT* curr_element = sf_hashset->elements[hash];
    while (curr_element != NULL) {
        if (strcmp(curr_element->value, value) == 0)
            return 1;
        curr_element = curr_element->next;
    }

    return 0;
}