#include "Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace
{

  class DeclCheck : public ASTVisitor
  {
    llvm::StringSet<> Scope;
    bool HasError;

    enum ErrorType
    {
      Twice,
      Not
    };

    void error(ErrorType ET, llvm::StringRef V)
    {
      llvm::errs() << "Variable " << V << " "
                   << (ET == Twice ? "already" : "not")
                   << " declared\n";
      HasError = true;
    }

  public:
    DeclCheck() : HasError(false) {}

    bool hasError() { return HasError; }

    virtual void visit(Factor &Node) override
    {
      if (Node.getKind() == Factor::Ident)
      {
        if (Scope.find(Node.getVal()) == Scope.end())
          error(Not, Node.getVal());
      }
    };

    virtual void visit(BinaryOp &Node) override
    {
      if (Node.getLeft())
        Node.getLeft()->accept(*this);
      else
        HasError = true;
      if (Node.getRight())
        Node.getRight()->accept(*this);
      else
        HasError = true;
    };

    virtual void visit(WithDecl &Node) override
    {
      for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
      {
        if (!Scope.insert(*I).second)
          error(Twice, *I);
      }
      if (Node.getExpr())
        Node.getExpr()->accept(*this);
      else
        HasError = true;
    };
  };

  class ZeroDivCheck : public ASTVisitor
  {
    bool HasError;
    bool DivParent;

    void error()
    {
      llvm::errs() << "Zero division\n";
      HasError = true;
    }

  public:
    ZeroDivCheck() : HasError(false) {}

    bool hasError() { return HasError; }

    virtual void visit(Factor &Node) override
    {
      if (DivParent)
        if (Node.getVal() == "0")
          error();
    }

    virtual void visit(BinaryOp &Node) override
    {
      DivParent = false;
      if (Node.getLeft())
        Node.getLeft()->accept(*this);
      else
        HasError = true;
      if (Node.getOperator() == BinaryOp::Div)
        DivParent = true;
      if (Node.getRight())
        Node.getRight()->accept(*this);
      else
        HasError = true;
    }

    virtual void visit(WithDecl &Node) override
    {
      if (Node.getExpr())
        Node.getExpr()->accept(*this);
      else
        HasError = true;
    }
  };

}

bool Sema::semantic(AST *Tree)
{
  if (!Tree)
    return false;
  DeclCheck Check;
  ZeroDivCheck ZeroCheck;
  Tree->accept(Check);
  Tree->accept(ZeroCheck);
  return Check.hasError() || ZeroCheck.hasError();
}
