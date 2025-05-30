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

Token Parser::peak(int n) {
  if (n >= tokens.size()) {
    Log::err("Token at index ", n, " is out of bounds");
  }

  return tokens.at(n);
};

Token Parser::eat() {
  changedLine = isNextTokenOnSameLine() ? false : true;
  
  Token token = this->tokens.front();
  this->tokens.erase(this->tokens.begin());
  return token;
};

Token Parser::expect(TokenType tokenType, const char* errorMessage) {
  if (this->tokens.front().type != tokenType) {
    Log::err(errorMessage);
  }

  changedLine = isNextTokenOnSameLine() ? false : true;

  // eat and retur token
  Token token = this->tokens.front();
  this->tokens.erase(this->tokens.begin());
  return token;
};

Token Parser::expect(TokenType tokenType, std::string& errorMessage) {
  
  if (this->tokens.front().type != tokenType) {
    Log::err(errorMessage);
  }

  changedLine = isNextTokenOnSameLine() ? false : true;

  // eat and retur token
  Token token = this->tokens.front();
  this->tokens.erase(this->tokens.begin());
  return token;
};

void Parser::expectOptionalSemicolon(std::string& message) {
  if (peak().type == TokenType::END_OF_FILE) {
    return;
  }
  
  if (peak().type == TokenType::SEMICOLON) {
    eat();
    return;
  }

  if (!changedLine) {
    Log::err(message);
  }
};

void Parser::expectOptionalSemicolon(const char * message) {
  if (peak().type == TokenType::END_OF_FILE) {
    return;
  }
  
  if (peak().type == TokenType::SEMICOLON) {
    eat();
    return;
  }

  if (!changedLine) {
    Log::err(message);
  }
};

bool Parser::isNextTokenOnSameLine() {
  if (tokens.size() < 2) return false; // case where there is only "END_OF_FILE" token
  
  return tokens[0].line == tokens[1].line; 
}

// MAIN FUNCTION
Program Parser::parse(std::string& filepath) {
  Lexer lexer = Lexer();
  this->tokens = lexer.tokenize(filepath);

  // DEBUG
  Log::log("TOKENS");
  for (auto token : tokens) {
    Log::log("{", token.type, " ,", token.value, " ,", token.line, "}");
  }
  // END DEBUG

  std::vector<Statement*> body;

  // Create program
  Program program = Program(body);

  while(peak().type != TokenType::END_OF_FILE) {
    program.body.push_back(parseStatement());
  }
  
  return program;
};


// PRIMARY EXPRESSIONS - numbers, strings, identifiers, parenthesis
Expression* Parser::parsePrimary() {
  TokenType type = peak().type;

  
  if (type == TokenType::NUMBER) return new NumericLiteral(eat().value);
  else if (type == TokenType::IDENTIFIER) return new Identifier(eat().value);
  else if (type == TokenType::STRING) return new StringLiteral(eat().value);
  else if (type == TokenType::BOOLEAN_TOKEN) return new BooleanLiteral(eat().value);
  else if (type == TokenType::NULL_TOKEN) { 
    eat(); // get past null token
    return new NullLiteral(); 
  } 
  else if (type == TokenType::OPEN_PARENT) {
    eat(); // consume parenthesis

    auto value = parseExpression();
    expect(TokenType::CLOSE_PARENT, "Expected ')' closing the expression");
    
    return value;
  } else {
    Log::err("Unexpected token found during parsing: ", type);
    
    return nullptr;
  }
}

// Orders Of Prescidence
// Assignment              0
// Object                  1
// AdditiveExpr            2
// MultiplicitaveExpr      3
// Call                    4
// Member                  5
// PrimaryExpr             6

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
  Statement* stmt = nullptr;

  if (peak().type == TokenType::IDENTIFIER && peak(1).type == TokenType::EQUAL) {
    stmt = parseVarAssignment();
  }
  else if (peak().type == TokenType::LET || peak().type == TokenType::CONST) {
    stmt = parseVarDeclaration();
  } else if (peak().type == TokenType::DEF) { 
    stmt = parseFunctionDeclaration();
  } else {
    stmt = parseExpression();
  }

  std::string message = "Expected semicolon ';' or new line after statement in line ";
  message = message + std::to_string(peak().line);
    
  expectOptionalSemicolon(message);
  
  return stmt;
};

Statement* Parser::parseVarDeclaration() {
  Token token = eat();
  bool isConstant = token.type == TokenType::CONST;

  Token ident = expect(TokenType::IDENTIFIER, "Expected Identifier symbol");
  
  if (peak().type != TokenType::EQUAL) {
    return new VarDeclaration(ident.value, new NullLiteral(), isConstant);
  }
  
  expect(TokenType::EQUAL, "Expexted equals character '='");

  Expression* value = parseExpression();

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

  }
  expect(TokenType::CLOSE_PARENT, "Expected closing parenthesis ')' in the end of function parameters declaration");

  // Get function body
  expect(TokenType::OPEN_BRACE, "Expected open brace for initializing function body");

  std::vector<Statement*> body;

  while(peak().type != TokenType::CLOSE_BRACE) {

    body.push_back(parseStatement());

  }

  expect(TokenType::CLOSE_BRACE, "Expected close brace '}' ending function body");

  return new FunctionDeclaration(funcIdent.value, params, body);
}

Statement* Parser::parseVarAssignment() {
  auto left = parsePrimary();

  if (peak().type != TokenType::EQUAL) {
    Log::err("Expected equal token '=' in variable assignment");  
  }

  auto ident = dynamic_cast<Identifier*>(left);
  if (!ident) {
    Log::err("Left hand side of assignment expression should be an identifier");
  }

  eat();

  auto right = parseExpression();

  return new VariableAssignment(ident->symbol, right);
};