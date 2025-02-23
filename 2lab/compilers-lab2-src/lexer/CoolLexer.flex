%{
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>

#include "Parser.h"
#include "CoolLexer.h"

#undef YY_DECL
#define YY_DECL int CoolLexer::yylex()  

%}

/* base symbols */
white_space       [ \n\t\f\r\t\v]*
lower             [a-z]
upper             [A-Z]
letter            {lower}|{upper}
underscore        _
digit             [0-9]
id_body           ({letter}|{underscore}|{digit})*

/* keywords */
class             (?i:class)
else              (?i:else)
false             f(?i:alse)
fi                (?i:fi)
if                (?i:if)
in                (?i:in)
inherits          (?i:inherits)
isvoid            (?i:isvoid)
let               (?i:let)
loop              (?i:loop)
pool              (?i:pool)
then              (?i:then)
while             (?i:while)
case              (?i:case)
esac              (?i:esac)
new               (?i:new)
of                (?i:of)
not               (?i:not)
true              t(?i:rue)

/* identifiers */
type              {upper}{id_body}
object            {lower}{id_body}

/* literals */
int               {digit}+
string            \"(.|\\\n)*\"
bad_string        \"(.|\n|\0)*\"

%x COMMENT


%option warn nodefault batch noyywrap c++
%option yylineno
%option yyclass="CoolLexer"

%%

{class}           return tok::kw_class;
{else}            return tok::kw_else;
{false}           return tok::kw_false;
{fi}              return tok::kw_fi;
{if}              return tok::kw_if;
{in}              return tok::kw_in;
{inherits}        return tok::kw_inherits;
{isvoid}          return tok::kw_isvoid;
{let}             return tok::kw_let;
{loop}            return tok::kw_loop;
{pool}            return tok::kw_pool;
{then}            return tok::kw_then;
{while}           return tok::kw_while;
{case}            return tok::kw_case;
{esac}            return tok::kw_esac;
{new}             return tok::kw_new;
{of}              return tok::kw_of;
{not}             return tok::kw_not;
{true}            return tok::kw_true;

{type}            return tok::id_type;
{object}          return tok::id_object;

{int}             return tok::lt_int;
{string}          return tok::lt_str;
{bad_string}      return tok::lt_bad_str;

\n                   lineno++;
{white_space}        { /* skip spaces */ }
.                    Error("unrecognized character");

%%

void CoolLexer::Error(const char* msg) const
{
    std::cerr << "Lexer error (line " << lineno << "): " << msg << ": lexeme '" << YYText() << "'\n";
    std::exit(YY_EXIT_FAILURE);
}
