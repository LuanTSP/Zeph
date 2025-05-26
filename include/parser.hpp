#pragma once
#include "lexer.hpp"
#include "token.hpp"
#include "node.hpp"
#include <vector>
#include <string>


class Parser {
  private:
  std::vector<Token> tokens;

  public:
  Parser();
  Token peak();
  Token eat();
  Token expect(TokenType tokenType, const char* errorMessage);
  Token expect(TokenType tokenType, std::string errorMessage);
  Program parse(std::string filepath);
  Expression* parsePrimary();
  Expression* parseExpression(int minPrec = -1);
};