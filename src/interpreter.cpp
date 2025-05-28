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
    delete left;
    delete right;

    return new NumberValue(result);
  }
  
  delete left;
  delete right;

  Log::err("Binary expression not supported for given types");
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