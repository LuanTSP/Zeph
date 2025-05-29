#include "../include/interpreter.hpp"

Interpreter::Interpreter() {};

RuntimeValue* Interpreter::evaluate(Statement* stmt, Enviroment& env) {
  switch (stmt->type) {
    case NodeType::PROGRAM: {
      auto program = dynamic_cast<Program*>(stmt);
      if (!program) {
        Log::err("Invalid cast to Program");
      }
      return evaluateProgram(program, env);
    }
    
    case NodeType::NUMERIC_LITERAL: {
      auto nLiteral = dynamic_cast<NumericLiteral*>(stmt);
      if (!nLiteral) {
        Log::err("Invalid cast to NumericLiteral");
      }
      return new NumberValue(std::stof(nLiteral->value));
    }

    case NodeType::IDENTIFIER_LITERAL: {
      auto ident = dynamic_cast<Identifier*>(stmt);
      if (!ident) {
        Log::err("Invalid cast to Identifier");
      }

      return evaluateIdentifier(ident, env);
    }

    case NodeType::STRING_LITERAL: {
      auto str = dynamic_cast<StringLiteral*>(stmt);
      if (!str) {
        Log::err("Invalid cast to StringLiteral");
      }

      return new StringValue(str->value);
    }

    case NodeType::NULL_LITERAL: {
      auto nll = dynamic_cast<NullLiteral*>(stmt);
      if (!nll) {
        Log::err("Invalid cast to NullLiteral");
      }

      return new NullValue();
    }

    case NodeType::BOOLEAN_LITERAL: {
      auto bll = dynamic_cast<BooleanLiteral*>(stmt);
      if (!bll) {
        Log::err("Invalid cast to BooleanLiteral");
      }

      bool value = bll->value == "true" ? true : false;
      return new BooleanValue(value);
    }

    case NodeType::VAR_DECLARATION: {
      auto decl = dynamic_cast<VarDeclaration*>(stmt);
      if (!decl) {
        Log::err("Invalid cast to VarDeclaration");
      }

      return evaluateVariableDeclaration(decl, env);
    }

    case NodeType::BINARY_EXPRESSION: {
      auto binExpr = dynamic_cast<BinaryExpression*>(stmt);
      if (!binExpr) {
        Log::err("Invalid cast to BinaryExpression");
      }
      return evaluateBinaryExpression(binExpr, env);
    }

    default:
      Log::err("This node has not been setup for interpretation: ", stmt->type);
      return new NullValue();
  }
}

RuntimeValue* Interpreter::evaluateVariableDeclaration(VarDeclaration* decl, Enviroment& env) {
  auto value = evaluate(decl->value, env);
  
  return env.declareVariable(decl->symbol, value, decl->isConstant);
}

RuntimeValue* Interpreter::evaluateIdentifier(Identifier* ident, Enviroment& env) {
  env = env.resolve(ident->symbol); // Get the envorment of the variable

  return env.variables[ident->symbol]; // Return the value
}

RuntimeValue* Interpreter::evaluateProgram(Program* program, Enviroment& env) {
  RuntimeValue* last = nullptr;

  for (auto stmt : program->body) {
    last = evaluate(stmt, env);
  }

  return last;
}

RuntimeValue* Interpreter::evaluateBinaryExpression(BinaryExpression* binExpr, Enviroment& env) {
  RuntimeValue* left = evaluate(binExpr->left, env);
  RuntimeValue* right = evaluate(binExpr->right, env);

  if (left->type == ValueType::NUMBER_VALUE && right->type == ValueType::NUMBER_VALUE) {
    float result = evaluateNumericBinaryExpression(
      static_cast<NumberValue*>(left)->value,
      static_cast<NumberValue*>(right)->value,
      binExpr->op
    );

    // Clean up intermediate values
    // delete left;
    // delete right;

    return new NumberValue(result);
    
  } else if (left->type == ValueType::BOOLEAN && right->type == ValueType::BOOLEAN) {
    bool bLeft = static_cast<BooleanValue*>(left)->value;
    bool bRight = static_cast<BooleanValue*>(right)->value;
    
    return new NumberValue(bLeft + bRight);

  } else if (left->type == ValueType::BOOLEAN && right->type == ValueType::NUMBER_VALUE) {
    bool bLeft = static_cast<BooleanValue*>(left)->value;
    float nRight = static_cast<NumberValue*>(right)->value;
    
    return new NumberValue(bLeft + nRight);

  } else if (left->type == ValueType::NUMBER_VALUE && right->type == ValueType::BOOLEAN) {
    float nLeft = static_cast<NumberValue*>(left)->value;
    bool bRight = static_cast<BooleanValue*>(right)->value;
    
    return new NumberValue(nLeft + bRight);

  } else if (left->type == ValueType::STRING_VALUE && right->type == ValueType::STRING_VALUE) {
    std::string sLeft = static_cast<StringValue*>(left)->value;
    std::string sRight = static_cast<StringValue*>(right)->value;
    return new StringValue(sLeft + sRight);

  } else if (left->type == ValueType::NULL_VALUE) {
    return right;

  } else if (right->type == NULL_VALUE) {
    return left;
  }
  
  delete left;
  delete right;

  Log::err("Binary expression not supported for given types");
  return nullptr;
}

float Interpreter::evaluateNumericBinaryExpression(float left, float right, const std::string& op) {
  if (op == "+") return left + right;
  if (op == "-") return left - right;
  if (op == "*") return left * right;
  if (op == "/") {
    if (right == 0.0f) {
      Log::err("Division by zero");
    }
    return left / right;
  }

  Log::err("Unknown numeric operator: " + op);
  return 0.0f;
}