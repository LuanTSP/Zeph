#pragma once
#include "lexer.hpp"
#include "token.hpp"
#include "node.hpp"
#include <vector>


class Parser {
  private:
  std::vector<Token> tokens;

  public:
  Parser();
  Token peak();
  Token eat();
  Token expect(TokenType tokenType, const char* errorMessage);
  Program parse(std::string filepath);
  
};