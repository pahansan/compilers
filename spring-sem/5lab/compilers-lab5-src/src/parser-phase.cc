#include "cool-tree.h"
#include "utilities.h"
#include "cool-parse.h"

#include "inheritance_graph.h"
#include "semant.h"

#include <unistd.h>
#include <cstdio>
#include <unordered_map>
#include <vector>
#include <set>
#include <iostream>

std::FILE *token_file = stdin;
extern Classes parse_results;
extern Program ast_root;

extern int curr_lineno;
char *curr_filename;
extern int parse_errors;

extern int yy_flex_debug;
extern int cool_yydebug;
int lex_verbose = 0;

extern int cool_yyparse();

int main(int argc, char **argv)
{
    yy_flex_debug = 0;
    cool_yydebug = 0;
    lex_verbose = 0;

    for (int i = 1; i < argc; i++)
    {
        curr_filename = argv[i];
        token_file = std::fopen(argv[i], "r");
        if (token_file == NULL)
        {
            std::cerr << "Error: can not open file " << argv[i] << std::endl;
            std::exit(1);
        }
        curr_lineno = 1;

        cool_yyparse();
        if (parse_errors != 0)
        {
            std::cerr << "Error: parse errors\n";
            std::exit(1);
        }

        ast_roots.push_back(ast_root);

        std::fclose(token_file);
    }
    semant();

    return 0;
}
