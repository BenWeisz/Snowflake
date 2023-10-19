cmake_minimum_required(VERSION 3.13)
project(snowflake)

set(SOURCE_FILES 
    main.c 
    src/snowflake.c
    src/util.c)

add_executable(snowflake ${SOURCE_FILES})