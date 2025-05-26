#pragma once
#include <string>
#include <vector>

enum NodeType {
  // Statements
  PROGRAM,
  // Expressions
  BINARY_EXPRESSIONS,
  // Literals
  NUMERIC_LITERALS,
  IDENT,
};

// Statements do not result in values at runtime
struct Statement {
  NodeType type;

  Statement(NodeType t) : type(t) {}
  virtual ~Statement() = default;
};

struct Program : Statement {
  std::vector<Statement> body;
  
  public:
  Program(std::vector<Statement> body) : Statement(NodeType::PROGRAM), body(body) {}
};

// Expressions result in values at runtime
struct Expression : Statement {
  Expression(NodeType t) : Statement(t) {}
};

struct Identifier : Expression {
  std::string symbol;
  
  public:
  Identifier(std::string symbol) : Expression(NodeType::IDENT), symbol(symbol) {};
};

struct BinaryExpression : Expression {
  std::string op;
  Expression left;
  Expression right;
  
  public:
  BinaryExpression(std::string op, Expression left, Expression right) : 
  Expression(NodeType::BINARY_EXPRESSIONS), 
  op(op), 
  left(left), 
  right(right) 
  {};
};

