#include "../include/parser.hpp"
#include "../include/log.hpp"
#include "../include/lexer.hpp"
#include "../include/node.hpp"
#include <memory>
#include <string>

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
  // Log::log("TOKENS");
  // for (auto token : tokens) {
  //   Log::log("{", token.type, " ,", token.value, " ,", token.line, "}");
  // }
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
  Expression* expr = nullptr;

  switch (peak().type) {
    
    case TokenType::NUMBER:
      expr = new NumericLiteral(eat().value);
      break;

    case TokenType::BINARY_OP: // Parses unary operations (-, +)
    Log::info(peak().value);
      if (peak().value != "+" && peak().value != "-") {
        Log::err("Unary operators can only be '+' and '-'");
      } 

      if (peak().value == "-") {
        eat();
        if (peak().type != TokenType::NUMBER) {
          Log::err("Expected a number after unary operator");
        }
        expr = new NumericLiteral("-" + eat().value);
        break;
      }

      eat();
      if (peak().type != TokenType::NUMBER) {
        Log::err("Expected a number after unary operator");
      }
      expr = new NumericLiteral(eat().value);
      break;

    case TokenType::IDENTIFIER:
      expr = new Identifier(eat().value);
      break;

    case TokenType::STRING:
      expr = new StringLiteral(eat().value);
      break;

    case TokenType::BOOLEAN_TOKEN:
      expr = new BooleanLiteral(eat().value);
      break;

    case TokenType::NULL_TOKEN:
      eat(); // consume null
      expr = new NullLiteral();
      break;

    case TokenType::OPEN_PARENT:
      eat(); // consume '('
      expr = parseExpression();
      expect(TokenType::CLOSE_PARENT, "Expected closing ')'");
      break;

    default:
      Log::err("Unexpected token in primary expression: ", peak().type);
  }

  // Handle possible call expression: identifier followed by '('
  while (peak().type == TokenType::OPEN_PARENT) {
    expr = parseCallExpression(expr);
  }

  return expr;
}

/* Orders Of Prescidence */
// PrimaryExpr / Call
// MultiplicitaveExpr
// AdditiveExpr
// Comparison

// COMPOUND EXPRESSIONS - result in values - binaryOps
Expression* Parser::parseMultiplicativeExpression() {
  Expression* left = parsePrimary();

  while (peak().type == TokenType::BINARY_OP && (peak().value == "*" || peak().value == "/" || peak().value == "%")) {
    std::string op = eat().value;
    Expression* right = parsePrimary(); // The right operand is another primary expression
    left = new BinaryExpression(op, left, right);
  }
  return left;
}

// Handles addition and subtraction
Expression* Parser::parseAdditiveExpression() {
  Expression* left = parseMultiplicativeExpression();

  while (peak().type == TokenType::BINARY_OP && (peak().value == "+" || peak().value == "-")) {
    std::string op = eat().value; // Consume the operator
    Expression* right = parseMultiplicativeExpression();
    left = new BinaryExpression(op, left, right);
  }
  return left;
}

// Handles comparison operators
Expression* Parser::parseComparisonExpression() {
  Expression* left = parseAdditiveExpression();

  while (peak().type == TokenType::COMPARISON) {
    std::string op = eat().value; // Consume the comparison operator
    Expression* right = parseAdditiveExpression();
    left = new ComparisonExpression(left, right, op);
  }
  return left;
}

Expression* Parser::parseExpression() {
  return parseComparisonExpression();
}

Expression* Parser::parseCallExpression(Expression* caller) {
  eat(); // Eat Open Parent '(' Token

  std::vector<Expression*> args;

  if (peak().type != TokenType::CLOSE_PARENT) {
    args.push_back(parseExpression());

    while (peak().type == TokenType::COMMA) {
      eat(); // consume comma
      args.push_back(parseExpression());
    }
  }

  expect(TokenType::CLOSE_PARENT, "Expected ')' after function arguments");

  return new CallExpression(caller, args);
}

// STATEMENTS - do not result in values - varDeclarations
Statement* Parser::parseStatement() {
  Statement* stmt = nullptr;

  if (peak().type == TokenType::IDENTIFIER && peak(1).type == TokenType::EQUAL) {
    stmt = parseVarAssignment();
  } else if (peak().type == TokenType::LET || peak().type == TokenType::CONST) {
    stmt = parseVarDeclaration();
  } else if (peak().type == TokenType::DEF) { 
    stmt = parseFunctionDeclaration();
  } else if (peak().type == TokenType::RETURN) { 
    stmt = parseReturnStatement();
  } else if (peak().type == TokenType::IF) { 
    stmt = parseIfStatement();
  } else if (peak().type == TokenType::WHILE) { 
    stmt = parseWhileStatement();
  } else if (peak().type == TokenType::BREAK) { 
    stmt = parseBreakStatement();
  } else if (peak().type == TokenType::CONTINUE) { 
    stmt = parseContinueStatement();
  } else {
    stmt = parseExpression();
  }

  std::string message = "Expected semicolon ';' or new line after statement in line ";
  message = message + std::to_string(peak().line);
    
  expectOptionalSemicolon(message);
  
  return stmt;
};

Statement* Parser::parseBreakStatement() {
  eat();

  return new BreakStatement();
}

Statement* Parser::parseContinueStatement() {
  eat();

  return new ContinueStatement();
}

Statement* Parser::parseWhileStatement() {
  eat(); // Eat while keyword
  expect(TokenType::OPEN_PARENT, "Expected an open parenthesis '(' after 'while' keyword");

  if (peak().type == TokenType::CLOSE_PARENT) {
    Log::err("Expected expression inside parenthesis '()' in line ", peak().line);
  }

  Expression* cond = parseExpression();
  if (!cond) {
    Log::err("Error parsing condition expression");
  }

  expect(TokenType::CLOSE_PARENT, "Expected close parenthesis, ')' after condition");

  expect(TokenType::OPEN_BRACE, "while statement body must start with open brace '{'");

  std::vector<Statement*> body;
  while(peak().type != TokenType::END_OF_FILE && peak().type != TokenType::CLOSE_BRACE) {
    body.push_back(parseStatement());
  }

  expect(TokenType::CLOSE_BRACE, "'while' statement body must end with close braces '}'");

  return new WhileStatement(cond, body);
};

Statement* Parser::parseIfStatement() {
  eat(); // Eat if keyword
  expect(TokenType::OPEN_PARENT, "Expected an open parenthesis '(' after 'if' keyword");

  if (peak().type == TokenType::CLOSE_PARENT) {
    Log::err("Expected expression inside parenthesis '()' in line ", peak().line);
  }

  Expression* cond = parseExpression();
  if (!cond) {
    Log::err("Error parsing condition expression");
  }

  expect(TokenType::CLOSE_PARENT, "Expected close parenthesis, ')' after condition");

  expect(TokenType::OPEN_BRACE, "if statement body must start with open brace '{'");

  std::vector<Statement*> ifBody;
  while(peak().type != TokenType::END_OF_FILE && peak().type != TokenType::CLOSE_BRACE) {
    ifBody.push_back(parseStatement());
  }

  expect(TokenType::CLOSE_BRACE, "'if' statement body must end with close braces '}'");

  std::vector<Statement*> elseBody; 
  if (peak().type == TokenType::ELSE) {
    eat();
    expect(TokenType::OPEN_BRACE, "Expected open brace '{' after 'else' keyword");

    while(peak().type != TokenType::END_OF_FILE && peak().type != TokenType::CLOSE_BRACE) {
      elseBody.push_back(parseStatement());
    }

    expect(TokenType::CLOSE_BRACE, "Expected close crace '}' when ending 'else' statement body");
  }

  return new IfStatement(cond, ifBody, elseBody);
};

Statement* Parser::parseReturnStatement() {
  auto returnTk = eat();

  if (peak().type == TokenType::SEMICOLON || peak().line != returnTk.line ) {
    return new ReturnStatement(new NullLiteral());
  }

  Expression* expr = parseExpression();
  return new ReturnStatement(expr);
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