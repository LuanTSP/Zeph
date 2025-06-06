#pragma once
#include <string>

enum TokenType {
  END_OF_FILE,
  IDENTIFIER,
  NUMBER,
  BOOLEAN_TOKEN,
  STRING,
  EQUAL,
  COMPARISON,
  LET,
  CONST,
  DEF,
  IF,
  ELSE,
  WHILE,
  BREAK,
  CONTINUE,
  RETURN,
  NULL_TOKEN,
  BINARY_OP,
  UNARY_OP,
  OPEN_BRACE,
  CLOSE_BRACE,
  OPEN_PARENT,
  CLOSE_PARENT,
  COMMA,
  SEMICOLON,
  DOT,
  DOUBLE_QUOTES,
};

struct Token {
  public:
  TokenType type;
  std::string value;
  int line;
  Token(TokenType type, std::string value, int line);
};