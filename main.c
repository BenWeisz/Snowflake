#include <stdio.h>

#include "io.h"
#include "ast.h"

int main() {
    SF_FILE* sf_file = sf_read_file("../samples/sample.sfd");
    if (sf_file != NULL) {
        _SF_AST_NODE* sf_ast = _sf_build_ast(sf_file);
    }

    return 0;
}