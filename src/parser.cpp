#include "../include/parser.hpp"
#include "../include/log.hpp"
#include "../include/lexer.hpp"
#include "../include/node.hpp"
#include <memory>

// CONSTRUCTOR 
Parser::Parser() {};

// HELPER FUNCTIONS
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

Token Parser::expect(TokenType tokenType, std::string& errorMessage) {
  if (this->tokens.front().type != tokenType) {
    Log::err(errorMessage);
  }
  // eat and retur token
  Token token = this->tokens.front();
  this->tokens.erase(this->tokens.begin());
  return token;
};

void Parser::expectOptionalSemicolon(std::string& message) {
  if (peak().type == TokenType::SEMICOLON) {
    while (peak().type == TokenType::SEMICOLON) {
      eat();
    }
    
    if (peak().type == TokenType::NEW_LINE) {
      while (peak().type == TokenType::NEW_LINE) {
        eat();
      }
    }
  } else {
    expect(TokenType::NEW_LINE, message);

    while(peak().type == TokenType::NEW_LINE) {
      eat();
    }
  }
};

void Parser::expectOptionalSemicolon(const char * message) {
  if (peak().type == TokenType::SEMICOLON) {
    while (peak().type == TokenType::SEMICOLON) {
      eat();
    }
    
    if (peak().type == TokenType::NEW_LINE) {
      while (peak().type == TokenType::NEW_LINE) {
        eat();
      }
    }
  } else {
    expect(TokenType::NEW_LINE, message);

    while(peak().type == TokenType::NEW_LINE) {
      eat();
    }
  }
};

// MAIN FUNCTION
Program Parser::parse(std::string& filepath) {
  Lexer lexer = Lexer();
  this->tokens = lexer.tokenize(filepath);

  // DEBUG
  Log::log("TOKENS");
  for (auto token : tokens) {
    Log::log("{", token.type, " ,", token.value, "}");
  }
  // END DEBUG

  std::vector<Statement*> body;

  // Create program
  Program program = Program(body);

  while(peak().type != TokenType::END_OF_FILE) {
    program.body.push_back(parseStatement());
  }

  for (auto stmt : program.body) {
    Log::log(stmt->type);
  }
  
  return program;
};


// PRIMARY EXPRESSIONS - numbers, strings, identifiers, parenthesis
Expression* Parser::parsePrimary() {
  TokenType type = peak().type;

  
  if (type == TokenType::NUMBER) return new NumericLiteral(eat().value);
  else if (type == TokenType::IDENTIFIER) return new Identifier(eat().value);
  else if (type == TokenType::STRING) return new StringLiteral(eat().value);
  else if (type == TokenType::OPEN_PARENT) {
    eat(); // consume parenthesis

    auto value = parseExpression();
    expect(TokenType::CLOSE_PARENT, "Expected ')' closing the expression");
    
    return value;
  } else {
    Log::err("Unexpected token found during parsing: ", type);
    
    // BUG ??
    return nullptr;
  }
}

// Orders Of Prescidence
// Assignment
// Object
// AdditiveExpr
// MultiplicitaveExpr
// Call
// Member
// PrimaryExpr

// COMPOUND EXPRESSIONS - result in values - binaryOps
Expression* Parser::parseExpression(int minPrec) {
  auto left = parsePrimary();

  while(peak().type != TokenType::END_OF_FILE && peak().type == TokenType::BINARY_OP) {
    // Determine precedence of operator
    int precedence;
    std::string op = peak().value;
    if (op == "+" || op == "-") precedence = 0;
    else if (op == "*" || op == "/" || op == "%") precedence = 1;
    else Log::err("Non recognized operation: ", op);

    if (precedence < minPrec) {
      break;
    }

    eat();

    auto right = parseExpression(precedence + 1);

    left = new BinaryExpression(op, left, right);
  }

  return left;
}

// STATEMENTS - do not result in values - varDeclarations
Statement* Parser::parseStatement() {
  TokenType type = peak().type;

  if (type == TokenType::LET || type == TokenType::CONST) return parseVarDeclaration();
  else if (type == TokenType::DEF) return parseFunctionDeclaration();
  else return parseExpression();
};

Statement* Parser::parseVarDeclaration() {
  Token token = eat();
  bool isConstant = token.type == TokenType::CONST;

  Token ident = expect(TokenType::IDENTIFIER, "Expected Identifier symbol");
  expect(TokenType::EQUAL, "Expexted equals character '='");

  Expression* value = parseExpression();

  if (peak().type == TokenType::END_OF_FILE) return new VarDeclaration(ident.value, value, isConstant);
  
  expectOptionalSemicolon("Variable declaration should be followed by new line or simicolon ';'");

  return new VarDeclaration(ident.value, value, isConstant);
};

Statement* Parser::parseFunctionDeclaration() {
  eat();
  auto funcIdent = expect(TokenType::IDENTIFIER, "Expected identifier for function declaration");

  expect(TokenType::OPEN_PARENT, "Expected '(' in order to initialize arguments for function declaration");

  // Get parameters list
  std::vector<std::string> params;

  if (peak().type != TokenType::CLOSE_PARENT) {
    auto param = expect(TokenType::IDENTIFIER, "Expected a parameter in function declaration");
    params.push_back(param.value);

    while(peak().type == TokenType::COMMA) {
      eat(); // eat comma
      param = expect(TokenType::IDENTIFIER, "Expected a parameter in function declaration");
      params.push_back(param.value);
    }

    expect(TokenType::CLOSE_PARENT, "Expected closing parenthesis ')' in the end of function parameters declaration");
  }

  // EAT ALL NEWLINES
  while(peak().type == TokenType::NEW_LINE) {
    eat(); // eat all newlines
  }

  // Get function body
  expect(TokenType::OPEN_BRACE, "Expected open brace for initializing function body");

  // EAT ALL NEWLINES
  while(peak().type == TokenType::NEW_LINE) {
    eat(); // eat all newlines
  }

  std::vector<Statement*> body;

  while(peak().type != TokenType::CLOSE_BRACE) {
    // EAT ALL NEWLINES
    while(peak().type == TokenType::NEW_LINE) {
      eat(); // eat all newlines
    }

    body.push_back(parseStatement());

    // EAT ALL NEWLINES
    while(peak().type == TokenType::NEW_LINE) {
      eat(); // eat all newlines
    }
  }

  expect(TokenType::CLOSE_BRACE, "Expected close brace '}' ending function body");

  if (peak().type == TokenType::END_OF_FILE) return new FunctionDeclaration(funcIdent.value, params, body);

  expectOptionalSemicolon("Expected newline or semicolon ';' ending function declaration");

  return new FunctionDeclaration(funcIdent.value, params, body);
}