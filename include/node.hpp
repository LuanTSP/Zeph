#pragma once
#include <string>
#include <vector>
#include "node.hpp"

// ALL NODE TYPES
enum NodeType {
  // Statements
  PROGRAM,
  VAR_DECLARATION,
  // Expressions
  BINARY_EXPRESSION,
  // Literals
  NUMERIC_LITERAL,
  STRING_LITERAL,
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

struct Identifier : Expression {
  public:
  std::string symbol;
  
  Identifier(std::string symbol) : Expression(NodeType::IDENTIFIER_LITERAL), symbol(symbol) {};
};

// Types of expression nodes
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