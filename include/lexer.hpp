#pragma once
#include <vector>
#include <string>
#include "token.hpp"

class Lexer {
  public:
  std::vector<Token> tokens;
  Lexer();
  std::vector<Token> tokenize(std::string filepath);
};