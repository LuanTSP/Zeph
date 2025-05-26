#include "../include/parser.hpp"
#include "../include/log.hpp"
#include "../include/lexer.hpp"
#include <memory>

Parser::Parser() {};
Token Parser::peak() {
  return tokens.front();
};

Token Parser::eat() {
  Token token = this->tokens.front();
  this->tokens.erase(this->tokens.begin());
  return token;
};

Token Parser::expect(TokenType tokenType, const char* errorMessage) {
  if (this->tokens.front().type != tokenType) {
    Log::err(errorMessage);
  }
  // eat and retur token
  Token token = this->tokens.front();
  this->tokens.erase(this->tokens.begin());
  return token;
};

Program Parser::parse(std::string filepath) {
  Lexer lexer = Lexer();
  this->tokens = lexer.tokenize(filepath);
  Log::log("Parsing...");

  std::vector<Statement> body;

  // while (peak().type != TokenType::END_OF_FILE) {
  //   auto tk = eat();
  //   Log::log(tk.type, " ", tk.value, " ", tokens.size());
  // }

  // Create program
  Program program = Program(body);
  return program;
};