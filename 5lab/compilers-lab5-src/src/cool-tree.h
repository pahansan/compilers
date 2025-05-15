#pragma once

#include "tree.h"
#include "cool-tree.handcode.h"

Symbol make_symbol(std::string str);

#include "tree.h"
#include "cool-tree.handcode.h"

class Program_class : public tree_node
{
public:
   Classes classes;

   Program_class(Classes a1)
   {
      classes = a1;
      type_ = "Program";
   }
   tree_node *copy() { return copy_Program(); }
   Program copy_Program();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);

   Classes get_classes();
};

class Class__class : public tree_node
{
public:
   Symbol parent;
   Features features;
   Symbol filename;

   Class__class(Symbol a1, Symbol a2, Features a3, Symbol a4)
   {
      name = a1;
      parent = a2;
      features = a3;
      filename = a4;
      type_ = "Class_";
   }

   tree_node *copy() { return copy_Class_(); }
   Class_ copy_Class_();
   void dump(std::ostream &stream, int n);
   Symbol get_filename() { return filename; }
   void dump_with_types(std::ostream &, int);

   Symbol get_parent();
   Features get_features();
};

class Formal_class : public tree_node
{
public:
   Symbol type_decl;

   Formal_class(Symbol a1, Symbol a2)
   {
      name = a1;
      type_decl = a2;
      type_ = "Formal";
   }
   Formal copy_Formal();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
   tree_node *copy() { return copy_Formal(); }
};

class Feature_class : public tree_node
{
public:
   tree_node *copy() { return copy_Feature(); }
   virtual Feature copy_Feature() = 0;
   virtual void dump_with_types(std::ostream &, int) = 0;
   std::string get_type() { return type_; }
};

class method_class : public Feature_class
{
public:
   Formals formals;
   Symbol return_type;
   Expression expr;

   method_class(Symbol a1, Formals a2, Symbol a3, Expression a4)
   {
      name = a1;
      formals = a2;
      return_type = a3;
      expr = a4;
      type_ = "method";
   }
   Feature copy_Feature();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class attr_class : public Feature_class
{
public:
   Symbol type_decl;
   Expression init;

   attr_class(Symbol a1, Symbol a2, Expression a3)
   {
      name = a1;
      type_decl = a2;
      init = a3;
      type_ = "attr";
   }
   Feature copy_Feature();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class Expression_class : public tree_node
{
public:
   Symbol type;
   tree_node *copy() { return copy_Expression(); }
   virtual Expression copy_Expression() = 0;
   Symbol get_type() { return type; }
   Expression set_type(Symbol s)
   {
      type = s;
      return this;
   }
   virtual void dump_with_types(std::ostream &, int) = 0;
   void dump_type(std::ostream &, int);
   Expression parent;
   virtual void set_body(const Expression e) {}
   Expression_class() { type = (Symbol)NULL; }
};

class assign_class : public Expression_class
{
public:
   Expression expr;

   assign_class(Symbol a1, Expression a2)
   {
      name = a1;
      expr = a2;
      type_ = "assign";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class static_dispatch_class : public Expression_class
{
public:
   Expression expr;
   Symbol type_name;
   Expressions actual;

   static_dispatch_class(Expression a1, Symbol a2, Symbol a3, Expressions a4)
   {
      expr = a1;
      type_name = a2;
      name = a3;
      actual = a4;
      type_ = "static_dispatch";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class dispatch_class : public Expression_class
{
public:
   Expression expr;
   Expressions actual;

   dispatch_class(Expression a1, Symbol a2, Expressions a3)
   {
      expr = a1;
      name = a2;
      actual = a3;
      type_ = "dispatch";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class cond_class : public Expression_class
{
public:
   Expression pred;
   Expression then_exp;
   Expression else_exp;

   cond_class(Expression a1, Expression a2, Expression a3)
   {
      pred = a1;
      then_exp = a2;
      else_exp = a3;
      type_ = "cond";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);

   void dump_with_types(std::ostream &, int);
};

class loop_class : public Expression_class
{
public:
   Expression pred;
   Expression body;

   loop_class(Expression a1, Expression a2)
   {
      pred = a1;
      body = a2;
      type_ = "loop";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class Case_class : public tree_node
{
public:
   Symbol type_decl;
   Expression expr;

   Case_class(Symbol a1, Symbol a2, Expression a3)
   {
      name = a1;
      type_decl = a2;
      expr = a3;
      type_ = "Case";
   }
   Case copy_Case();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
   tree_node *copy() { return copy_Case(); }
};

class typcase_class : public Expression_class
{
public:
   Expression expr;
   Cases cases;

   typcase_class(Expression a1, Cases a2)
   {
      expr = a1;
      cases = a2;
      type_ = "typcase";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class block_class : public Expression_class
{
public:
   Expressions body;

   block_class(Expressions a1)
   {
      body = a1;
      type_ = "block";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class let_class : public Expression_class
{
public:
   Symbol identifier;
   Symbol type_decl;
   Expression init;
   Expression body;

   let_class(Symbol a1, Symbol a2, Expression a3, Expression a4)
   {
      identifier = a1;
      type_decl = a2;
      init = a3;
      body = a4;
      type_ = "let";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
   void set_body(const Expression e) override
   {
      e->parent = this;
      body = e;
   }
};

class plus_class : public Expression_class
{
public:
   Expression e1;
   Expression e2;

   plus_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
      type_ = "plus";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class sub_class : public Expression_class
{
public:
   Expression e1;
   Expression e2;

   sub_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
      type_ = "sub";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class mul_class : public Expression_class
{
public:
   Expression e1;
   Expression e2;

   mul_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
      type_ = "mul";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class divide_class : public Expression_class
{
public:
   Expression e1;
   Expression e2;

   divide_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
      type_ = "divide";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class neg_class : public Expression_class
{
public:
   Expression e1;

   neg_class(Expression a1)
   {
      e1 = a1;
      type_ = "neg";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class lt_class : public Expression_class
{
public:
   Expression e1;
   Expression e2;

   lt_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
      type_ = "lt";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class eq_class : public Expression_class
{
public:
   Expression e1;
   Expression e2;

   eq_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
      type_ = "eq";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class leq_class : public Expression_class
{
public:
   Expression e1;
   Expression e2;

   leq_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
      type_ = "leq";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class comp_class : public Expression_class
{
public:
   Expression e1;

   comp_class(Expression a1)
   {
      e1 = a1;
      type_ = "comp";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class int_const_class : public Expression_class
{
public:
   Symbol token;

   int_const_class(Symbol a1)
   {
      token = a1;
      type = make_symbol("Int");
      type_ = "int_const";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class bool_const_class : public Expression_class
{
public:
   Boolean val;

   bool_const_class(Boolean a1)
   {
      val = a1;
      type = make_symbol("Bool");
      type_ = "bool_const";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class string_const_class : public Expression_class
{
public:
   Symbol token;

   string_const_class(Symbol a1)
   {
      token = a1;
      type = make_symbol("String");
      type_ = "string_const";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class new__class : public Expression_class
{
public:
   Symbol type_name;

   new__class(Symbol a1)
   {
      type_name = a1;
      type_ = "new_";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class isvoid_class : public Expression_class
{
public:
   Expression e1;

   isvoid_class(Expression a1)
   {
      e1 = a1;
      type_ = "isvoid";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class no_expr_class : public Expression_class
{
public:
   no_expr_class()
   {
      type_ = "no_expr";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class object_class : public Expression_class
{
public:
   object_class(Symbol a1)
   {
      name = a1;
      type_ = "object";
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

Classes nil_Classes();
Classes single_Classes(Class_);
Classes append_Classes(Classes, Classes);
Features nil_Features();
Features single_Features(Feature);
Features append_Features(Features, Features);
Formals nil_Formals();
Formals single_Formals(Formal);
Formals append_Formals(Formals, Formals);
Expressions nil_Expressions();
Expressions single_Expressions(Expression);
Expressions append_Expressions(Expressions, Expressions);
Cases nil_Cases();
Cases single_Cases(Case);
Cases append_Cases(Cases, Cases);
Program program(Classes);
Class_ class_(Symbol, Symbol, Features, Symbol);
Feature method(Symbol, Formals, Symbol, Expression);
Feature attr(Symbol, Symbol, Expression);
Formal formal(Symbol, Symbol);
Case branch(Symbol, Symbol, Expression);
Expression assign(Symbol, Expression);
Expression static_dispatch(Expression, Symbol, Symbol, Expressions);
Expression dispatch(Expression, Symbol, Expressions);
Expression cond(Expression, Expression, Expression);
Expression loop(Expression, Expression);
Expression typcase(Expression, Cases);
Expression block(Expressions);
Expression let(Symbol, Symbol, Expression, Expression);
Expression plus(Expression, Expression);
Expression sub(Expression, Expression);
Expression mul(Expression, Expression);
Expression divide(Expression, Expression);
Expression neg(Expression);
Expression lt(Expression, Expression);
Expression eq(Expression, Expression);
Expression leq(Expression, Expression);
Expression comp(Expression);
Expression int_const(Symbol);
Expression bool_const(Boolean);
Expression string_const(Symbol);
Expression new_(Symbol);
Expression isvoid(Expression);
Expression no_expr();
Expression object(Symbol);

Symbol make_symbol(std::string str);
Class_ make_object_class();
Class_ make_io_class();
Class_ make_int_class();
Class_ make_string_class();
Class_ make_bool_class();
