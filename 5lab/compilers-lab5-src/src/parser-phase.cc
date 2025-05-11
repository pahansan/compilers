#include "cool-tree.h"
#include "utilities.h"
#include "cool-parse.h"

#include "inheritance_graph.h"

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

        /* TODO: dump AST tree (ast_root) to std::cerr */
        // ast_root->dump_with_types(std::cerr, 0);

        // stringtable.print();
        // inttable.print();
        // idtable.print();

        Classes classes = ast_root->classes;

        // for (int i = 0; classes->more(i); i = classes->next(i))
        // {
        //     Class_ class_ = classes->nth(i);
        //     std::cout << class_->name->get_string() << ':' << class_->parent->get_string() << '\n';
        //     if (std::string(classes->nth(i)->get_name()->get_string()) == "Main")
        //     {
        //         Features features = classes->nth(i)->features;
        //         for (int i = 0; features->more(i); i = features->next(i))
        //         {
        //             std::cout << features->nth(i)->name->get_string() << ':';
        //             std::cout << features->nth(i)->type_ << ':';
        //             if (features->nth(i)->type_ == "method")
        //                 std::cout << features->nth(i)->return_type << '\n';
        //             else
        //                 std::cout << features->nth(i)->type_decl << '\n';
        //         }
        //     }
        // }
        // std::cout << '\n';
        std::fclose(token_file);

        make_inheritance_graph();
    }

    return 0;
}
