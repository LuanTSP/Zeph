#pragma once
#include "lexer.hpp"
#include "token.hpp"
#include "node.hpp"
#include <vector>
#include <string>


class Parser {
  private:
  std::vector<Token> tokens;
  bool changedLine = false;

  public:
  Parser();
  Token peak();
  Token peak(int n);
  Token eat();
  Token expect(TokenType tokenType, const char* errorMessage);
  Token expect(TokenType tokenType, std::string& errorMessage);
  void expectOptionalSemicolon(std::string& errorMessage);
  void expectOptionalSemicolon(const char * errorMessage);
  bool isNextTokenOnSameLine();
  Program parse(std::string& filepath);
  Expression* parsePrimary();
  Expression* parseExpression();
  Expression* parseCallExpression(Expression* caller);
  Expression* parseComparisonExpression();
  Expression* parseAdditiveExpression();
  Expression* parseMultiplicativeExpression();
  Expression* parseAndExpression();
  Expression* parseOrExpression();
  Statement* parseStatement();
  Statement* parseReturnStatement();
  Statement* parseVarDeclaration();
  Statement* parseFunctionDeclaration();
  Statement* parseVarAssignment();
  Statement* parseIfStatement();
  Statement* parseWhileStatement();
  Statement* parseBreakStatement();
  Statement* parseContinueStatement();
};