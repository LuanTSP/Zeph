#pragma once
#include <string>

enum TokenType {
  END_OF_FILE,
  NEW_LINE,
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
  OPEN_BRACKET,
  CLOSE_BRACKET,
  OPEN_PARENT,
  CLOSE_PARENT,
  COMMA,
  DOT,
  DOUBLE_QUOTES,
};

struct Token {
  Token(TokenType type, std::string value);
  TokenType type;
  std::string value;
};