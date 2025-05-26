#pragma once
#include <string>
#include <vector>

enum NodeType {
  // Statements
  PROGRAM,
  // Expressions
  BINARY_EXPRESSION,
  // Literals
  NUMERIC_LITERAL,
  IDENT,
};


// Statements do not result in values at runtime
struct Statement {
  public:
  NodeType type;

  Statement(NodeType t) : type(t) {}
  virtual ~Statement() = default;
};

struct Program : Statement {
  public:
  std::vector<Statement*> body;
  
  Program(std::vector<Statement*> body) : Statement(NodeType::PROGRAM), body(body) {}
};

// Expressions result in values at runtime
struct Expression : Statement {
  Expression(NodeType t) : Statement(t) {}
};

struct Identifier : Expression {
  public:
  std::string symbol;
  
  Identifier(std::string symbol) : Expression(NodeType::IDENT), symbol(symbol) {};
};

struct NumericLiteral : Expression {
  public:
  std::string value;
  
  NumericLiteral(std::string value) : Expression(NodeType::NUMERIC_LITERAL), value(value) {};
};

struct BinaryExpression : Expression {
  public:
  std::string op;
  Expression *left;
  Expression *right;

  BinaryExpression(std::string op, Expression *left, Expression *right) : Expression(NodeType::BINARY_EXPRESSION), left(left), right(right), op(op) {};
};