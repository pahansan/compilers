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
    case (tok::op_dot):
        return "op_dot";
    case (tok::op_at):
        return "op_at";
    case (tok::op_tilda):
        return "op_tilda";
    case (tok::op_asterisk):
        return "op_asterisk";
    case (tok::op_slash):
        return "op_slash";
    case (tok::op_arrow):
        return "op_arrow";
    case (tok::op_plus):
        return "op_plus";
    case (tok::op_minus):
        return "op_minus";
    case (tok::op_less_eq):
        return "op_less_eq";
    case (tok::op_less):
        return "op_less";
    case (tok::op_eq):
        return "op_eq";
    case (tok::sp_colon):
        return "sp_colon";
    case (tok::sp_semi_colon):
        return "sp_semi_colon";
    case (tok::sp_comma):
        return "sp_comma";
    case (tok::br_r_open):
        return "br_r_open";
    case (tok::br_r_close):
        return "br_r_close";
    case (tok::br_c_open):
        return "br_c_open";
    case (tok::br_c_close):
        return "br_c_close";

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
    std::string tmp = "\\a";
    std::string tmp2 = "a";
    for (char s = 'A'; s <= 'z'; s++)
    {
        if (s == 'b' || s == 't' || s == 'n' || s == 'f')
            continue;
        if ((s >= 'a' && s <= 'z') || (s >= 'A' && s <= 'Z'))
        {
            tmp[1] = s;
            tmp2[0] = s;
            replaceAllSubstrings(str, tmp, tmp2);
        }
    }
    str.erase(0, 1);
    str.erase(str.size() - 1);
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
        std::cout << std::setw(3) << std::left << lexer->line() << ':';
        if (token == tok::lt_str)
            std::cout << "Token: " << std::setw(12) << std::left << get_token_name(token) << " '" << format_lt_str(lexer->YYText()) << "'\n";
        else
            std::cout << "Token: " << std::setw(12) << std::left << get_token_name(token) << " '" << lexer->YYText() << "'\n";
    }

    return 0;
}
