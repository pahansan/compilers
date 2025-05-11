#ifndef COOL_TREE_H
#define COOL_TREE_H

#include "tree.h"
#include "cool-tree.handcode.h"

typedef class Program_class *Program;

class Program_class : public tree_node
{
protected:
   Classes classes;

public:
   Program_class(Classes a1)
   {
      classes = a1;
   }
   tree_node *copy() { return copy_Program(); }
   Program copy_Program();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);

   Classes get_classes();
};

typedef class Class__class *Class_;

class Class__class : public tree_node
{
protected:
   
   Symbol parent;
   Features features;
   Symbol filename;

public:
   Class__class(Symbol a1, Symbol a2, Features a3, Symbol a4)
   {
      name = a1;
      parent = a2;
      features = a3;
      filename = a4;
   }
   tree_node *copy() { return copy_Class_(); }
   Class_ copy_Class_();
   void dump(std::ostream &stream, int n);
   Symbol get_filename() { return filename; }
   void dump_with_types(std::ostream &, int);

   Symbol get_parent();
   Features get_features();
};

typedef class Feature_class *Feature;

class Feature_class : public tree_node
{
public:
   tree_node *copy() { return copy_Feature(); }
   virtual Feature copy_Feature() = 0;
   virtual void dump_with_types(std::ostream &, int) = 0;
};

typedef class Formal_class *Formal;

class Formal_class : public tree_node
{
protected:
   
   Symbol type_decl;

public:
   Formal_class(Symbol a1, Symbol a2)
   {
      name = a1;
      type_decl = a2;
   }
   Formal copy_Formal();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
   tree_node *copy() { return copy_Formal(); }
};

typedef class Expression_class *Expression;

class Expression_class : public tree_node
{
public:
   tree_node *copy() { return copy_Expression(); }
   virtual Expression copy_Expression() = 0;
   Symbol type;
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

typedef class Case_class *Case;

class Case_class : public tree_node
{
protected:
   
   Symbol type_decl;
   Expression expr;

public:
   Case_class(Symbol a1, Symbol a2, Expression a3)
   {
      name = a1;
      type_decl = a2;
      expr = a3;
   }
   Case copy_Case();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
   tree_node *copy() { return copy_Case(); }
};

typedef list_node<Class_> Classes_class;
typedef Classes_class *Classes;

typedef list_node<Feature> Features_class;
typedef Features_class *Features;

typedef list_node<Formal> Formals_class;
typedef Formals_class *Formals;

typedef list_node<Expression> Expressions_class;
typedef Expressions_class *Expressions;

typedef list_node<Case> Cases_class;
typedef Cases_class *Cases;

class method_class : public Feature_class
{
protected:
   
   Formals formals;
   Symbol return_type;
   Expression expr;

public:
   method_class(Symbol a1, Formals a2, Symbol a3, Expression a4)
   {
      name = a1;
      formals = a2;
      return_type = a3;
      expr = a4;
   }
   Feature copy_Feature();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class attr_class : public Feature_class
{
protected:
   
   Symbol type_decl;
   Expression init;

public:
   attr_class(Symbol a1, Symbol a2, Expression a3)
   {
      name = a1;
      type_decl = a2;
      init = a3;
   }
   Feature copy_Feature();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class assign_class : public Expression_class
{
protected:
   
   Expression expr;

public:
   assign_class(Symbol a1, Expression a2)
   {
      name = a1;
      expr = a2;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class static_dispatch_class : public Expression_class
{
protected:
   Expression expr;
   Symbol type_name;
   
   Expressions actual;

public:
   static_dispatch_class(Expression a1, Symbol a2, Symbol a3, Expressions a4)
   {
      expr = a1;
      type_name = a2;
      name = a3;
      actual = a4;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class dispatch_class : public Expression_class
{
protected:
   Expression expr;
   
   Expressions actual;

public:
   dispatch_class(Expression a1, Symbol a2, Expressions a3)
   {
      expr = a1;
      name = a2;
      actual = a3;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class cond_class : public Expression_class
{
protected:
   Expression pred;
   Expression then_exp;
   Expression else_exp;

public:
   cond_class(Expression a1, Expression a2, Expression a3)
   {
      pred = a1;
      then_exp = a2;
      else_exp = a3;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);

   void dump_with_types(std::ostream &, int);
};

class loop_class : public Expression_class
{
protected:
   Expression pred;
   Expression body;

public:
   loop_class(Expression a1, Expression a2)
   {
      pred = a1;
      body = a2;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class typcase_class : public Expression_class
{
protected:
   Expression expr;
   Cases cases;

public:
   typcase_class(Expression a1, Cases a2)
   {
      expr = a1;
      cases = a2;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class block_class : public Expression_class
{
protected:
   Expressions body;

public:
   block_class(Expressions a1)
   {
      body = a1;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class let_class : public Expression_class
{
protected:
   Symbol identifier;
   Symbol type_decl;
   Expression init;
   Expression body;

public:
   let_class(Symbol a1, Symbol a2, Expression a3, Expression a4)
   {
      identifier = a1;
      type_decl = a2;
      init = a3;
      body = a4;
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
protected:
   Expression e1;
   Expression e2;

public:
   plus_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class sub_class : public Expression_class
{
protected:
   Expression e1;
   Expression e2;

public:
   sub_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class mul_class : public Expression_class
{
protected:
   Expression e1;
   Expression e2;

public:
   mul_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class divide_class : public Expression_class
{
protected:
   Expression e1;
   Expression e2;

public:
   divide_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class neg_class : public Expression_class
{
protected:
   Expression e1;

public:
   neg_class(Expression a1)
   {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class lt_class : public Expression_class
{
protected:
   Expression e1;
   Expression e2;

public:
   lt_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class eq_class : public Expression_class
{
protected:
   Expression e1;
   Expression e2;

public:
   eq_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class leq_class : public Expression_class
{
protected:
   Expression e1;
   Expression e2;

public:
   leq_class(Expression a1, Expression a2)
   {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class comp_class : public Expression_class
{
protected:
   Expression e1;

public:
   comp_class(Expression a1)
   {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class int_const_class : public Expression_class
{
protected:
   Symbol token;

public:
   int_const_class(Symbol a1)
   {
      token = a1;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class bool_const_class : public Expression_class
{
protected:
   Boolean val;

public:
   bool_const_class(Boolean a1)
   {
      val = a1;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class string_const_class : public Expression_class
{
protected:
   Symbol token;

public:
   string_const_class(Symbol a1)
   {
      token = a1;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class new__class : public Expression_class
{
protected:
   Symbol type_name;

public:
   new__class(Symbol a1)
   {
      type_name = a1;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class isvoid_class : public Expression_class
{
protected:
   Expression e1;

public:
   isvoid_class(Expression a1)
   {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class no_expr_class : public Expression_class
{
protected:
public:
   no_expr_class()
   {
   }
   Expression copy_Expression();
   void dump(std::ostream &stream, int n);
   void dump_with_types(std::ostream &, int);
};

class object_class : public Expression_class
{
protected:
   

public:
   object_class(Symbol a1)
   {
      name = a1;
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

#endif
