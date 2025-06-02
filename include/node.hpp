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
  // Compound Expressions
  BINARY_EXPRESSION,
  CALL_EXPRESSION,
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