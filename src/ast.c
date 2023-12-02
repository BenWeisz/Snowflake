#include "ast.h"

_SF_AST_NODE* _sf_build_ast(const SF_FILE* sf_file) {
    // Copy out the sf_file data so tokenizing doesn't clobber sf_file->data
    char* data = (char*)malloc(sizeof(char) * sf_file->size);
    if (data == NULL) {
        LOG_ERROR("ERROR: Failed to allocate memory for temp buffer\n");
        return NULL;
    }

    strncpy(data, sf_file->data, sf_file->size);

    char* curr_line = strtok(data, "\r\n");
    _SF_AST_NODE* root_node = NULL;

    // Build the root node if there are 1 or more lines
    if (curr_line == NULL) {
        LOG_ERROR("ERROR: The SF_FILE is empty.\n");
        free(data);
        return NULL;
    }
    else {
        root_node = _sf_new_ast_node();
        root_node->line = 0;
        if (root_node == NULL) return NULL;
    }

    // Parse the data line by line
    int line_i = 1;
    int last_indent = -1;
    _SF_AST_NODE* curr_node = root_node;
    char* line_buffer = (char*)malloc(sizeof(char) * _SF_LINE_BUFFER_SIZE);
    while (curr_line != NULL) {
        // Set-up the line buffer for line parsing
        unsigned int line_len = strlen(curr_line);
        strncpy(line_buffer, curr_line, line_len + 1);

        if (_sf_is_blank_line(line_buffer) == 0) {
            // Determine the line indent
            int indent = _sf_compute_indent(curr_line, line_i);
            if (indent == -1) {
                _sf_free_ast(root_node);
                return NULL;
            }

            // Add a child
            if (indent > last_indent) {
                // Build the structure
                curr_node->child = _sf_new_ast_node();
                if (curr_node->child == NULL) {
                    _sf_free_ast(root_node);
                    return NULL;
                }
                curr_node->child->parent = curr_node;
                curr_node->child->line = line_i;
                int r = _sf_parse_line(curr_node->child, line_buffer);
                if (r == 1) {
                    _sf_free_ast(root_node);
                    return NULL;
                }

                // Progress
                curr_node = curr_node->child;
            }
            // Add a sibling
            else if (indent == last_indent) {
                // Build the structure
                curr_node->sibling = _sf_new_ast_node();
                if (curr_node->sibling == NULL) {
                    _sf_free_ast(root_node);
                    return NULL;
                }
                curr_node->sibling->parent = curr_node->parent;
                curr_node->sibling->line = line_i;
                int r = _sf_parse_line(curr_node->sibling, line_buffer);
                if (r == 1) {
                    _sf_free_ast(root_node);
                    return NULL;
                }

                // Progress
                curr_node = curr_node->sibling;
            }
            // Move up
            else {
                // Build the structure
                curr_node->parent->sibling = _sf_new_ast_node();
                if (curr_node->parent->sibling == NULL) {
                    _sf_free_ast(root_node);
                    return NULL;
                }
                curr_node->parent->sibling->parent = curr_node->parent->parent;
                curr_node->parent->sibling->line = line_i;
                int r = _sf_parse_line(curr_node->parent->sibling, line_buffer);
                if (r == 1) {
                    _sf_free_ast(root_node);
                    return NULL;
                }

                // Progress
                curr_node = curr_node->parent->sibling;
            }
            
            last_indent = indent;
        }

        // FIX THIS: This line might end in /r/n or /n
        curr_line = strtok(curr_line + line_len + 1, "\r\n");
        line_i++;
    }

    // Free the temp buffer
    free(data);
    free(line_buffer);

    return root_node;
}

void _sf_free_ast(const _SF_AST_NODE* sf_anode) {
    
}

_SF_AST_NODE* _sf_new_ast_node() {
    _SF_AST_NODE* sf_anode = (_SF_AST_NODE*)malloc(sizeof(_SF_AST_NODE));
    if (sf_anode == NULL) {
        LOG_ERROR("ERROR: Failed to create new _SF_AST_NODE.\n");
        return NULL;
    }

    // Initialize the new _SF_AST_NODE
    sf_anode->type = _SF_AST_NODE_TYPE_ERROR;
    sf_anode->key[0] = 0x00;
    sf_anode->key[_SF_MAX_KEY_LEN] = 0x00;
    sf_anode->value[0] = 0x00;
    sf_anode->value[_SF_MAX_VALUE_LEN] = 0x00;
    sf_anode->line = -1;
    sf_anode->parent = NULL;
    sf_anode->child = NULL;
    sf_anode->sibling = NULL;

    return sf_anode;
}

int _sf_parse_line(_SF_AST_NODE* sf_anode, char* line_buffer) {
    char* curr_token = strtok(line_buffer, " \t");
    
    // Copy out the key str
    strcpy(sf_anode->key, curr_token);

    // Try to get the value str
    curr_token = strtok(NULL, " \t");
    if (curr_token != NULL) {
        strcpy(sf_anode->value, curr_token);
        sf_anode->type = _SF_AST_NODE_TYPE_ATTRIB;
    } 
    else {
        if (_sf_is_snowflake_line(sf_anode->key) == 1) {
            sf_anode->type = _SF_AST_NODE_TYPE_SNOWFLAKE;
        }
        else if (_sf_is_array_object_line(sf_anode->key) == 1) {
            sf_anode->type = _SF_AST_NODE_TYPE_ARRAY_OBJECT;
        } else {
            LOG_ERROR("ERROR: Line %d uses an invalid identifier.\n", sf_anode->line);
            return 1;
        }
    }
    return 0;
}

int _sf_compute_indent(const char* line, const int line_i) {
    int tabs = 0;
    int spaces = 0;

    // Count the occurance of spaces and tabs
    const char* curr_char = line;
    while (*curr_char == ' ' || *curr_char == '\t') {
        if (*curr_char == ' ')
            spaces++;
        else
            tabs++;
        curr_char++;
    }

    if (spaces % 4 != 0) {
        LOG_ERROR("Line %d is improperly indented, please use spaces in multiples of 4 or tabs.\n", line_i);
        return -1;
    }

    return tabs + (spaces / 4);
}

int _sf_is_blank_line(const char* line) {
    const char* curr_char = line;
    while (*curr_char != '\0') {
        if (*curr_char != ' ' && *curr_char != '\t' && *curr_char != '*')
            return 0;
        curr_char++;
    }
    return 1;
}

int _sf_is_snowflake_line(const char* line) {
    int line_len = strlen(line);
    if (line_len <= 2)
        return 0;
    else if (line[0] != '*' || line[line_len - 1] != '*')
        return 0;

    unsigned int char_i = 1;
    while (char_i < line_len - 1) {
        if ((line[char_i] < 'A' || line[char_i] > 'Z') && line[char_i] != '_')
            return 0;
        char_i++;
    }
    return 1;
}

int _sf_is_array_object_line(const char* line) {
    int line_len = strlen(line);
    if (line_len == 0)
        return 0;

    const char* curr_char = line;
    while (*curr_char != '\0') {
        if ((*curr_char < 'a' || *curr_char > 'z') && *curr_char != '-')
            return 0;
        curr_char++;
    }
    return 1;
}