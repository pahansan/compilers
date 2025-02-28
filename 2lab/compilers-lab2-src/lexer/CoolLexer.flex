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
white_space             [ \t\f\r\t\v]*
lower                   [a-z]
upper                   [A-Z]
letter                  {lower}|{upper}
underscore              \_
digit                   [0-9]
id_body                 ({letter}|{underscore}|{digit})*

/* keywords */
class                   (?i:class)
else                    (?i:else)
false                   f(?i:alse)
fi                      (?i:fi)
if                      (?i:if)
in                      (?i:in)
inherits                (?i:inherits)
isvoid                  (?i:isvoid)
let                     (?i:let)
loop                    (?i:loop)
pool                    (?i:pool)
then                    (?i:then)
while                   (?i:while)
case                    (?i:case)
esac                    (?i:esac)
new                     (?i:new)
of                      (?i:of)
not                     (?i:not)
true                    t(?i:rue)

/* identifiers */
type                    {upper}{id_body}
object                  {lower}{id_body}

/* literals */
int                     {digit}+

/* operators */
dot                     \.
at                      \@
tilda                   \~
asterisk                \*
slash                   \/
arrow                   \<\-
plus                    \+
minus                   \-
less_eq                 \<\=
less                    \<
eq                      \=

/* special symbols */
colon                   \:
semi_colon              \;
big_arrow               \=\>
comma                   \,

/* brackets */
r_open                  \(
r_close                 \)
c_open                  \{
c_close                 \}

com_open                \(\*
com_close               \*\)

%x COMMENT STRING

%option warn batch noyywrap c++
%option yylineno
%option yyclass="CoolLexer"

%%
{com_open}              {BEGIN(COMMENT); comment_level = 1;}
<COMMENT>{com_open}     {comment_level++;}
<COMMENT>.              { /* skip*/ }
<COMMENT>\n             { lineno++; }
<COMMENT><<EOF>>        Error("EOF in comment");
<COMMENT>{com_close}    {
                            comment_level--;
                            if(comment_level == 0)
                            BEGIN(INITIAL);
                        };

--.*$                   {}
com_open                Error("Comment close without open");

"\""                    { BEGIN(STRING); yymore(); }
<STRING>\n              { Error("Strings may not contain \\n"); BEGIN(INITIAL); lineno++; }
<STRING>\\0             { Error("Strings may not contain \\0"); BEGIN(INITIAL); lineno++; }
<STRING><<EOF>>         { Error("Strings may not contain EOF"); BEGIN(INITIAL); lineno++; }
<STRING>[^\\\"\n]*      { yymore(); }
<STRING>\\[^\n]         { yymore(); }
<STRING>\\\n            { lineno++; yymore(); }
<STRING>"\""            { BEGIN(INITIAL); return tok::lt_str; }

{class}                 return tok::kw_class;
{else}                  return tok::kw_else;
{false}                 return tok::kw_false;
{fi}                    return tok::kw_fi;
{if}                    return tok::kw_if;
{in}                    return tok::kw_in;
{inherits}              return tok::kw_inherits;
{isvoid}                return tok::kw_isvoid;
{let}                   return tok::kw_let;
{loop}                  return tok::kw_loop;
{pool}                  return tok::kw_pool;
{then}                  return tok::kw_then;
{while}                 return tok::kw_while;
{case}                  return tok::kw_case;
{esac}                  return tok::kw_esac;
{new}                   return tok::kw_new;
{of}                    return tok::kw_of;
{not}                   return tok::kw_not;
{true}                  return tok::kw_true;

{type}                  return tok::id_type;
{object}                return tok::id_object;

{int}                   return tok::lt_int;

{dot}                   return tok::op_dot;
{at}                    return tok::op_at;
{tilda}                 return tok::op_tilda;
{asterisk}              return tok::op_asterisk;
{slash}                 return tok::op_slash;
{arrow}                 return tok::op_arrow;
{plus}                  return tok::op_plus;
{minus}                 return tok::op_minus;
{less_eq}               return tok::op_less_eq;
{less}                  return tok::op_less;
{eq}                    return tok::op_eq;

{colon}                 return tok::sp_colon;
{semi_colon}            return tok::sp_semi_colon;
{big_arrow}             return tok::sp_big_arrow;
{comma}                 return tok::sp_comma;

{r_open}                return tok::br_r_open;
{r_close}               return tok::br_r_close;
{c_open}                return tok::br_c_open;
{c_close}               return tok::br_c_close;


\n                      lineno++;
{white_space}           { /* skip spaces */ }
.                       Error("unrecognized character");

%%

void CoolLexer::Error(const char* msg) const
{
    std::cerr << "Lexer error (line " << lineno << "): " << msg << ": lexeme '" << YYText() << "'\n";
    std::exit(YY_EXIT_FAILURE);
}
