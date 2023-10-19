#include <stdio.h>

#include "include/snowflake.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("ERROR: Please provide exactly one parameter: A snowflake file with extension .sf\n");
        return 1;
    }

    SNOWFLAKES snowflakes;
    load_snowflakes(argv[1], &snowflakes);
}