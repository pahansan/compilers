#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <algorithm>

#include "CoolLexer.h"
#include "Parser.h"

std::string get_token_name(int token)
{
    switch (token)
    {
    case (tok::kw_class):
        return "kw_class";
    case (tok::kw_else):
        return "kw_else";
    case (tok::kw_false):
        return "kw_false";
    case (tok::kw_fi):
        return "kw_fi";
    case (tok::kw_if):
        return "kw_if";
    case (tok::kw_in):
        return "kw_in";
    case (tok::kw_inherits):
        return "kw_inherits";
    case (tok::kw_isvoid):
        return "kw_isvoid";
    case (tok::kw_let):
        return "kw_let";
    case (tok::kw_loop):
        return "kw_loop";
    case (tok::kw_pool):
        return "kw_pool";
    case (tok::kw_then):
        return "kw_then";
    case (tok::kw_while):
        return "kw_while";
    case (tok::kw_case):
        return "kw_case";
    case (tok::kw_esac):
        return "kw_esac";
    case (tok::kw_new):
        return "kw_new";
    case (tok::kw_of):
        return "kw_of";
    case (tok::kw_not):
        return "kw_not";
    case (tok::kw_true):
        return "kw_true";
    case (tok::id_type):
        return "id_type";
    case (tok::id_object):
        return "id_object";
    case (tok::lt_int):
        return "lt_int";
    case (tok::lt_str):
        return "lt_str";
    case (tok::lt_bad_str):
        return "lt_bad_str";
    default:
        return "error_token";
    }
}

void replaceAllSubstrings(std::string &str, const std::string &from, const std::string &to)
{
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos)
    {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

std::string format_lt_str(std::string str)
{
    replaceAllSubstrings(str, "\b", "\\b");
    replaceAllSubstrings(str, "\t", "\\t");
    replaceAllSubstrings(str, "\n", "\\n");
    replaceAllSubstrings(str, "\f", "\\f");
    return str;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: coolc <source>\n";
        std::exit(EXIT_FAILURE);
    }
    std::ifstream ifs(argv[1]);
    if (ifs.fail())
    {
        std::cerr << "Error opening file `" << argv[1] << "`\n";
        std::exit(EXIT_FAILURE);
    }

    CoolLexer *lexer = new CoolLexer(ifs, std::cout);
    for (int token = lexer->yylex(); token; token = lexer->yylex())
    {
        if (token == tok::lt_str || token == tok::lt_bad_str)
            std::cout << "Token: " << std::setw(12) << std::left << get_token_name(token) << " '" << format_lt_str(lexer->YYText()) << "'\n";
        else
            std::cout << "Token: " << std::setw(12) << std::left << get_token_name(token) << " '" << lexer->YYText() << "'\n";
    }

    return 0;
}
