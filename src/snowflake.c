#include "../include/snowflake.h"

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

    curr_pos = 0;
    snowflake_i = -1;
    while (curr_pos < file_size) {
        line_end_pos = get_line_end_pos(buffer + curr_pos);

        if (is_snowflake_line(buffer + curr_pos) == 0) {
            snowflake_i++;
        }
        else if (is_snowflake_attrib_line(buffer + curr_pos, 1) == 1) {
            
        }

        curr_pos += line_end_pos + 1;
    }

    // Cleanup
    fclose(file);
}