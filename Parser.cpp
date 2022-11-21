#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

class ExprAST{
public:
    virtual ~ExprAST(){}
};

class NumExprAST : public ExprAST{
    double __Val;
public:
    NumExprAST(double val): __Val(val) {}
};

class VarExprAST : public ExprAST{
    std::string __Name;
public:
    VarExprAST(const string &name) : __Name(name) {}
};

class BinExprAST : public ExprAST{
    char __op;
    ExprAST *__LHS, *__RHS;
public:
    BinExprAST(char op,ExprAST *lhs, ExprAST *rhs) : __op(op),__LHS(lhs),__RHS(rhs) {}
};

class CallExprAST : public ExprAST{
    std::string __Callee;
    std::vector<ExprAST*> __Args;
public:
    CallExprAST(std::string &callee, std::vector<ExprAST*> Args) : __Callee(callee),__Args(Args) {}
};

class PrototypeAST{
    std::string __Name;
    std::vector<std::string> __Args;
public:
    PrototypeAST(std::string &name,std::vector<std::string> Args) : __Name(name),__Args(args) {}
};

class FunctionAST{
    PrototypeAST *__prototype;
    ExprAST *__body;
public:
    FunctionAST(PrototypeAST *prototype,ExprAST *body) : __prototype(prototype),__body(body) {}
};

static int CurTok;
static int getNextToken() {
  return CurTok = gettok();
}

static std::map<char, int> BinopPrecedence;

static int GetTokPrecedence() {
  if (!isascii(CurTok))
    return -1;

  int TokPrec = BinopPrecedence[CurTok];
  if (TokPrec <= 0) return -1;
  return TokPrec;
}

ExprAST *Error(const char *Str) { fprintf(stderr, "Error: %s\n", Str);return 0;}
PrototypeAST *ErrorP(const char *Str) { Error(Str); return 0; }
FunctionAST *ErrorF(const char *Str) { Error(Str); return 0; }

static ExprAST *ParseExpression();

static ExprAST *ParseIdentifierExpr() {
  std::string IdName = IdentifierStr;

  getNextToken(); 

  if (CurTok != '(')
    return new VariableExprAST(IdName);

  getNextToken(); 
  std::vector<ExprAST*> Args;
  if (CurTok != ')') {
    while (1) {
      ExprAST *Arg = ParseExpression();
      if (!Arg) return 0;
      Args.push_back(Arg);

      if (CurTok == ')') break;

      if (CurTok != ',')
        return Error("Expected ')' or ',' in argument list");
      getNextToken();
    }
  }

  getNextToken();

  return new CallExprAST(IdName, Args);
}

static ExprAST *ParseNumberExpr() {
  ExprAST *Result = new NumberExprAST(NumVal);
  getNextToken(); 
  return Result;
}

static ExprAST *ParseParenExpr() {
  getNextToken();
  ExprAST *V = ParseExpression();
  if (!V) return 0;

  if (CurTok != ')')
    return Error("expected ')'");
  getNextToken(); 
  return V;
}

static ExprAST *ParsePrimary() {
  switch (CurTok) {
  default: return Error("unknown token when expecting an expression");
  case tok_identifier: return ParseIdentifierExpr();
  case tok_number:     return ParseNumberExpr();
  case '(':            return ParseParenExpr();
  }
}

static ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS) {
  while (1) {
    int TokPrec = GetTokPrecedence();

    if (TokPrec < ExprPrec)
      return LHS;

    int BinOp = CurTok;
    getNextToken();  

    ExprAST *RHS = ParsePrimary();
    if (!RHS) return 0;.
    int NextPrec = GetTokPrecedence();
    if (TokPrec < NextPrec) {
      RHS = ParseBinOpRHS(TokPrec+1, RHS);
      if (RHS == 0) return 0;
    }

    LHS = new BinaryExprAST(BinOp, LHS, RHS);
  }
}

static ExprAST *ParseExpression() {
  ExprAST *LHS = ParsePrimary();
  if (!LHS) return 0;

  return ParseBinOpRHS(0, LHS);
}

static PrototypeAST *ParsePrototype() {
  if (CurTok != tok_identifier)
    return ErrorP("Expected function name in prototype");

  std::string FnName = IdentifierStr;
  getNextToken();

  if (CurTok != '(')
    return ErrorP("Expected '(' in prototype");

  std::vector<std::string> ArgNames;
  while (getNextToken() == tok_identifier)
    ArgNames.push_back(IdentifierStr);
  if (CurTok != ')')
    return ErrorP("Expected ')' in prototype");

  getNextToken(); 

  return new PrototypeAST(FnName, ArgNames);
}

static FunctionAST *ParseDefinition() {
  getNextToken();  
  PrototypeAST *Proto = ParsePrototype();
  if (Proto == 0) return 0;

  if (ExprAST *E = ParseExpression())
    return new FunctionAST(Proto, E);
  return 0;
}

static FunctionAST *ParseTopLevelExpr() {
  if (ExprAST *E = ParseExpression()) {
    PrototypeAST *Proto = new PrototypeAST("", std::vector<std::string>());
    return new FunctionAST(Proto, E);
  }
  return 0;
}

static PrototypeAST *ParseExtern() {
  getNextToken(); 
  return ParsePrototype();
}

static void HandleDefinition() {
  if (ParseDefinition()) {
    fprintf(stderr, "Parsed a function definition.\n");
  } else {
    getNextToken();
  }
}

static void HandleExtern() {
  if (ParseExtern()) {
    fprintf(stderr, "Parsed an extern\n");
  } else {.
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  if (ParseTopLevelExpr()) {
    fprintf(stderr, "Parsed a top-level expr\n");
  } else {
    getNextToken();
  }
}

static void MainLoop() {
  while (1) {
    fprintf(stderr, "ready> ");
    switch (CurTok) {
    case tok_eof:    return;
    case ';':        getNextToken(); break;  
    case tok_def:    HandleDefinition(); break;
    case tok_extern: HandleExtern(); break;
    default:         HandleTopLevelExpression(); break;
    }
  }
}