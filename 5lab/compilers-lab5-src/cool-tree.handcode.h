//
// The following include files must come first.

#ifndef COOL_TREE_HANDCODE_H
#define COOL_TREE_HANDCODE_H

#include <iostream>
#include "tree.h"
#include "stringtab.h"

#define yylineno curr_lineno

/* a type renaming */
typedef int Boolean;
class Entry;
typedef Entry *Symbol;

extern int yylineno;

inline Boolean copy_Boolean(Boolean b) { return b; }
inline void assert_Boolean(Boolean) {}
inline void dump_Boolean(std::ostream &stream, int padding, Boolean b)
{
	stream << pad(padding) << (int)b << "\n";
}

void dump_Symbol(std::ostream &stream, int padding, Symbol b);
void assert_Symbol(Symbol b);
Symbol copy_Symbol(Symbol b);

typedef class Program_class *Program;
typedef class Class__class *Class_;
typedef class Feature_class *Feature;
typedef class Formal_class *Formal;
typedef class Expression_class *Expression;
typedef class Case_class *Case;

typedef list_node<Class_> *Classes;
typedef list_node<Feature> *Features;
typedef list_node<Formal> *Formals;
typedef list_node<Expression> *Expressions;
typedef list_node<Case> *Cases;

#endif // ifndef COOL_TREE_HANDCODE_H
