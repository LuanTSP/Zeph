#pragma once
#include <string>
#include <vector>
#include "node.hpp"

// ALL NODE TYPES
enum NodeType {
  // Statements
  PROGRAM,
  VAR_DECLARATION,
  VAR_ASSIGNMENT,
  FUNC_DECLARATION,
  RETURN_STATEMENT,
  IF_STATEMENT,
  // Compound Expressions
  BINARY_EXPRESSION,
  CALL_EXPRESSION,
  COMPARISON_EXPRESSION,
  // Literal Expresisons
  NULL_LITERAL,
  NUMERIC_LITERAL,
  STRING_LITERAL,
  BOOLEAN_LITERAL,
  IDENTIFIER_LITERAL,
};


// Statements do not result in values at runtime
struct Statement {
  public:
  NodeType type;

  Statement(NodeType t) : type(t) {}
  virtual ~Statement() = default;
};

// Expressions result in values at runtime
struct Expression : Statement {
  Expression(NodeType t) : Statement(t) {}
};

// STATEMENTS //
struct Program : Statement {
  public:
  std::vector<Statement*> body;
  
  Program(std::vector<Statement*> body) : Statement(NodeType::PROGRAM), body(body) {}
};

struct VarDeclaration : Statement {
  public:
  std::string symbol;
  Expression* value;
  bool isConstant;

  VarDeclaration(std::string symbol, Expression* value, bool isConstant) : Statement(NodeType::VAR_DECLARATION), symbol(symbol), value(value), isConstant(isConstant) {}
};

struct IfStatement : Statement {
  public:
  Expression* cond = nullptr;
  std::vector<Statement*> ifBody;
  std::vector<Statement*> elseBody;

  IfStatement(Expression* cond, std::vector<Statement*> ifBody, std::vector<Statement*> elseBody) : Statement(NodeType::IF_STATEMENT), cond(cond), ifBody(ifBody), elseBody(elseBody) {};
};

struct FunctionDeclaration : Statement {
  public:
  std::string name;
  std::vector<std::string> params;
  std::vector<Statement*> body;

  FunctionDeclaration(std::string name, std::vector<std::string> params, std::vector<Statement*> body) : Statement(NodeType::FUNC_DECLARATION), name(name), params(params), body(body) {}
};

struct VariableAssignment : Statement {
  public:
  std::string ident;
  Expression* expr = nullptr;

 VariableAssignment(std::string ident, Expression* expr) : Statement(NodeType::VAR_ASSIGNMENT), ident(ident), expr(expr) {};
};

struct ReturnStatement : Statement {
  public:
  Expression* value = nullptr;

  ReturnStatement(Expression* value) : Statement(NodeType::RETURN_STATEMENT), value(value) {};
};

// EXPRESSIONS //
struct Identifier : Expression {
  public:
  std::string symbol;
  
  Identifier(std::string symbol) : Expression(NodeType::IDENTIFIER_LITERAL), symbol(symbol) {};
};

struct NullLiteral : Expression {
  public:
  std::string value = "null";

  NullLiteral() : Expression(NodeType::NULL_LITERAL) {};
};

struct BooleanLiteral : Expression {
  public:
  std::string value;

  BooleanLiteral(std::string value) : Expression(NodeType::BOOLEAN_LITERAL), value(value) {};
};

struct NumericLiteral : Expression {
  public:
  std::string value;
  
  NumericLiteral(std::string value) : Expression(NodeType::NUMERIC_LITERAL), value(value) {};
};

struct StringLiteral : Expression {
  public:
  std::string value;
  
  StringLiteral(std::string value) : Expression(NodeType::STRING_LITERAL), value(value) {};
};

struct BinaryExpression : Expression {
  public:
  std::string op;
  Expression *left;
  Expression *right;

  BinaryExpression(std::string op, Expression *left, Expression *right) : Expression(NodeType::BINARY_EXPRESSION), left(left), right(right), op(op) {};
};

struct CallExpression : Expression {
  public:
  Expression* caller;  // Typically an Identifier
  std::vector<Expression*> arguments;

  CallExpression(Expression* caller, std::vector<Expression*> args)
    : Expression(NodeType::CALL_EXPRESSION), caller(caller), arguments(args) {}
};

struct ComparisonExpression : Expression {
  public:
  Expression* lhs = nullptr;
  Expression* rhs = nullptr;

  ComparisonExpression(Expression* lhs, Expression* rhs) : Expression(NodeType::COMPARISON_EXPRESSION), lhs(lhs), rhs(rhs) {};
};