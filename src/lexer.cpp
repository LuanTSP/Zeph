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
  int line = 1;
  while (file.get(c)) {
    if (c == '\n') {
      // Check for newline token before check for white spaces
      line++;

      while (file.peek() != EOF && file.peek() == '\n') {
        line++;
        file.get(c);
      }

      continue;
    }
    else if (isspace(c)) {
      // Check for white spaces
      continue;
    } else if (c == '#') {
      // Check for comments
      while (file.peek() != EOF && file.peek() != '\n') {
        file.get(c);
      }
      if (file.peek() == '\n') line++;
      file.get(c);
    } else if (c == ';') {
      tokens.push_back(Token(TokenType::SEMICOLON, ";", line));

      while (file.peek() != EOF && file.peek() == ';') {
        file.get(c);
      }

      continue;
    } else if (c == '.') {
      tokens.push_back(Token(TokenType::DOT, ".", line));
    } else if (c == ',') {
      tokens.push_back(Token(TokenType::COMMA, ",", line));
    } else if (c == '{') {
      tokens.push_back(Token(TokenType::OPEN_BRACE, "{", line));
    } else if (c == '}') {
      tokens.push_back(Token(TokenType::CLOSE_BRACE, "}", line));
    } else if (c == '(') {
      tokens.push_back(Token(TokenType::OPEN_PARENT, "(", line));
    } else if (c == ')') {
      tokens.push_back(Token(TokenType::CLOSE_PARENT, ")", line));
    } else if (c == '=') {
      if (file.peek() == '=') {
        // Separate '=' / '=='
        file.get(c);
        tokens.push_back(Token(TokenType::COMPARISON, "==", line));
        continue;
      }

      tokens.push_back(Token(TokenType::EQUAL, "=", line));
    } else if (c == '<') {
      if (file.peek() == '=') {
        // Separate '<' / '<='
        file.get(c);
        tokens.push_back(Token(TokenType::COMPARISON, "<=", line));
        continue;
      }

      tokens.push_back(Token(TokenType::COMPARISON, "<", line));
    } else if (c == '>') {
      if (file.peek() == '=') {
        // Separate '>' / '>='
        file.get(c);
        tokens.push_back(Token(TokenType::COMPARISON, ">=", line));
        continue;
      }

      tokens.push_back(Token(TokenType::COMPARISON, ">", line));
    } else if (c == '"') {
      std::string str = "";
      file.get(c); // advance "
      
      while (file.peek() != EOF && c != '"') {
        str += c;
        file.get(c);
      }

      tokens.push_back(Token(TokenType::STRING, str, line));
    }  else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
      std::string op; op += c;
      tokens.push_back(Token(TokenType::BINARY_OP, op, line));
    } else if (std::isdigit(c)) {
      // Make numbers
      std::string number;
      number += c;

      while (file.peek() != EOF && std::isdigit(file.peek())) {
        file.get(c);
        number += c;
      }

      if (file.peek() == '.') {
        file.get(c);
        number += c;

        while (file.peek() != EOF && std::isdigit(file.peek())) {
          file.get(c);
          number += c;
        }
      }

      tokens.push_back(Token(TokenType::NUMBER, number, line));

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
        tokens.push_back(Token(TokenType::LET, ident, line));
      } else if (ident == "const") {
        tokens.push_back(Token(TokenType::CONST, ident, line));
      } else if (ident == "def") {
        tokens.push_back(Token(TokenType::DEF, ident, line));
      } else if (ident == "return") {
        tokens.push_back(Token(TokenType::RETURN, ident, line));
      } else if (ident == "null") {
        tokens.push_back(Token(TokenType::NULL_TOKEN, ident, line));
      } else if (ident == "true" || ident == "false") {
        tokens.push_back(Token(TokenType::BOOLEAN_TOKEN, ident, line));
      } else if (ident == "if") {
        tokens.push_back(Token(TokenType::IF, ident, line));
      } else if (ident == "else") {
        tokens.push_back(Token(TokenType::ELSE, ident, line));
      } else if (ident == "while") {
        tokens.push_back(Token(TokenType::WHILE, ident, line));
      } else {
        tokens.push_back(Token(TokenType::IDENTIFIER, ident, line));
      }
    } else {
      Log::err("Unrecognized character: ", c);
    }
  }

  // Add eof token in the very end
  tokens.push_back(Token(TokenType::END_OF_FILE, "EOF", line));

  return this->tokens;
}