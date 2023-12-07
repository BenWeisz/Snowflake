#include <stdio.h>

#include "io.h"
#include "ast.h"
#include "hash.h"

int main() {
    // SF_FILE* sf_file = sf_read_file("../samples/sample.sfd");
    // if (sf_file != NULL) {
    //     _SF_AST_NODE* sf_ast = _sf_build_ast(sf_file);
    //     _sf_free_ast(sf_ast);
    // }

    // sf_close_file(sf_file);

    // printf("%lu\n", _sf_hash("name", 7));
    // printf("%lu\n", _sf_hash("age", 7));
    // printf("%lu\n", _sf_hash("gpa", 7));
    // printf("%lu\n", _sf_hash("num_courses", 7));

    _SF_HASHSET* sf_hashset = _sf_hashset_init(7);

    _sf_hashset_add_value(sf_hashset, "a");
    _sf_hashset_add_value(sf_hashset, "b");
    _sf_hashset_add_value(sf_hashset, "c");
    _sf_hashset_add_value(sf_hashset, "d");
    _sf_hashset_add_value(sf_hashset, "e");
    _sf_hashset_add_value(sf_hashset, "f");
    _sf_hashset_add_value(sf_hashset, "g");

    int r = _sf_hashset_has_value(sf_hashset, "z");

    _sf_hashset_free(sf_hashset);

    return 0;
}