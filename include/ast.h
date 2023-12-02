#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "log.h"

#define _SF_AST_NODE_TYPE_ERROR 0x00
#define _SF_AST_NODE_TYPE_ATTRIB 0x01
#define _SF_AST_NODE_TYPE_ARRAY_OBJECT 0x02
#define _SF_AST_NODE_TYPE_SNOWFLAKE 0x03

// These two can be determined at generation time based on the .sf file
#define _SF_MAX_KEY_LEN 128
#define _SF_MAX_VALUE_LEN 1024
#define _SF_LINE_BUFFER_SIZE 1024 * 1024 // If this is too small you can always increase this

typedef struct _SF_AST_NODE {
    char type;
    char key[_SF_MAX_KEY_LEN + 1];
    char value[_SF_MAX_VALUE_LEN + 1];
    int line;
    struct _SF_AST_NODE* parent;
    struct _SF_AST_NODE* sibling;
    struct _SF_AST_NODE* child;
} _SF_AST_NODE;

_SF_AST_NODE* _sf_build_ast(const SF_FILE* sf_file);
void _sf_free_ast(const _SF_AST_NODE* sf_anode);

_SF_AST_NODE* _sf_new_ast_node();

/*
    Read in the key / value of lines, remove * where appropriate. 
    Set the key / value of sf_anode, and determine its type.
    Return 1 on error, 0 otherwise.
*/
int _sf_parse_line(_SF_AST_NODE* sf_anode, char* line_buffer);

/* 
    Counts 4 spaces as 1 indent, 1 tab as 1 indent.
    Returns -1 if line is partially indented.
*/
int _sf_compute_indent(const char* line, const int line_i);

/*
    Determine if this line is a blank line (only whitespace or * characters)
*/
int _sf_is_blank_line(const char* line);

/*
    Determine if this line is a snowflake line.
    ex. *STUDENT_LIST*
    ex. *APPLES*
*/
int _sf_is_snowflake_line(const char* line);

/*
    Determine if this line is an array or object line.
    ex. student_names
    ex. apples
*/
int _sf_is_array_object_line(const char* line);

#endif  // AST_H