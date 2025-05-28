#pragma once
#include <string>

enum TokenType {
  END_OF_FILE,
  IDENTIFIER,
  NUMBER,
  STRING,
  EQUAL,
  LET,
  CONST,
  DEF,
  RETURN,
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
  int col;
  Token(TokenType type, std::string value, int line);
};