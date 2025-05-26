#include "../include/lexer.hpp"
#include "../include/token.hpp"
#include "../include/log.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <cctype>  // for isdigit()

Lexer::Lexer() {}

std::vector<Token> Lexer::tokenize(std::string filepath) {
  std::ifstream file(filepath);

  if (!file) {
    Log::err("Error opening file: ", filepath);  
    std::exit(EXIT_FAILURE);
  }

  char c;
  while (file.get(c)) {
    if (isspace(c)) {
      // Check for whitespaces
      continue;
    } else if (c == '#') {
      // Check for comments
      while (file.peek() != EOF && file.peek() != '\n') {
        file.get(c);
      }
      file.get(c);
    } else if (c == '.') {
      tokens.push_back(Token(TokenType::DOT, "."));
    } else if (c == ',') {
      tokens.push_back(Token(TokenType::COMMA, ","));
    } else if (c == '{') {
      tokens.push_back(Token(TokenType::OPEN_BRACKET, "{"));
    } else if (c == '}') {
      tokens.push_back(Token(TokenType::CLOSE_BRACKET, "}"));
    } else if (c == '(') {
      tokens.push_back(Token(TokenType::OPEN_PARENT, "("));
    } else if (c == ')') {
      tokens.push_back(Token(TokenType::CLOSE_PARENT, ")"));
    } else if (c == '=') {
      tokens.push_back(Token(TokenType::EQUAL, "="));
    } else if (c == '"') {
      std::string str = "";
      str += c;
      file.get(c);

      while (file.peek() != EOF && c != '"') {
        str += c;
        file.get(c);
      }

      str += c;

      tokens.push_back(Token(TokenType::STRING, str));
    }  else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
      std::string op; op += c;
      tokens.push_back(Token(TokenType::BINARY_OP, op));
    } else if (std::isdigit(c)) {
      // Make numbers
      std::string number;
      number += c;

      while (file.peek() != EOF && std::isdigit(file.peek())) {
        file.get(c);
        number += c;
      }
      tokens.push_back(Token(TokenType::NUMBER, number));

    } else if (std::isalpha(c)) {
      // Make Identifiers and keywords
      std::string ident;
      ident += c;

      while (file.peek() != EOF && std::isalnum(file.peek())) {
        file.get(c);
        ident += c;
      }

      // Check for predefined keywords
      if (ident == "let") {
        tokens.push_back(Token(TokenType::LET, ident));
      } else if (ident == "const") {
        tokens.push_back(Token(TokenType::CONST, ident));
      } else if (ident == "def") {
        tokens.push_back(Token(TokenType::DEF, ident));
      } else if (ident == "return") {
        tokens.push_back(Token(TokenType::RETURN, ident));
      } else {
        tokens.push_back(Token(TokenType::IDENTIFIER, ident));
      }
    } else {
      Log::err("Unrecognized character: ", c);
    }
  }

  // Add eof token in the very end
  tokens.push_back(Token(TokenType::END_OF_FILE, "EOF"));

  return this->tokens;
}