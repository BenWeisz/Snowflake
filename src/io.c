#include "io.h"

SF_FILE* sf_read_file(const char* path) {
    // Open the file
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        LOG_ERROR("ERROR: Failed to open file \"%s\"\n", path);
        return NULL;
    }
    
    // Allocate memory the SF_FILE
    SF_FILE* sf_file = (SF_FILE*)malloc(sizeof(SF_FILE));
    if (sf_file == NULL) {
        LOG_ERROR("ERROR: Failed to allocate memory for SF_FILE\n");
        fclose(file);
        return NULL;
    }

    // Measure the file size
    fseek(file, 0, SEEK_END);
    sf_file->size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate room for the file's data
    sf_file->data = (char*)malloc(sizeof(char) * sf_file->size);
    if (sf_file->data == NULL) {
        LOG_ERROR("ERROR: Failed to allocate memory for SF_FILE\n");
        free(sf_file);
        fclose(file);
        return NULL;
    }

    // Read the data from the file
    if (fread(sf_file->data, sizeof(char), sf_file->size, file) != sf_file->size) {
        LOG_ERROR("ERROR: Failed to read all of the bytes from \"%s\"\n", path);
        free(sf_file->data);
        free(sf_file);
        fclose(file);
        return NULL;
    }

    return sf_file;
}

void sf_close_file(SF_FILE* sf_file) {
    free(sf_file->data);
    free(sf_file);
}