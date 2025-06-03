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

      bool value = bll->value == "true" ? 1 : 0;
      return new BooleanValue(value);
    }

    case NodeType::VAR_DECLARATION: {
      auto decl = dynamic_cast<VarDeclaration*>(stmt);
      if (!decl) {
        Log::err("Invalid cast to VarDeclaration");
      }

      return evaluateVariableDeclaration(decl, env);
    }

    case NodeType::VAR_ASSIGNMENT: {
      auto assign = dynamic_cast<VariableAssignment*>(stmt);
      if (!assign) {
        Log::err("Invalid cast to VariableAssignment");
      }

      return evaluateVariableAssignment(assign, env);
    }

    case NodeType::BINARY_EXPRESSION: {
      auto binExpr = dynamic_cast<BinaryExpression*>(stmt);
      if (!binExpr) {
        Log::err("Invalid cast to BinaryExpression");
      }
      return evaluateBinaryExpression(binExpr, env);
    }

    case NodeType::FUNC_DECLARATION: {
      auto fDecl = dynamic_cast<FunctionDeclaration*>(stmt);
      if (!fDecl) {
        Log::err("Invalid cast to FunctionDeclaration");
      }
      return evaluateFunctionDeclaration(fDecl, env);
    }

    case NodeType::CALL_EXPRESSION: {
      auto callExpr = dynamic_cast<CallExpression*>(stmt);
      if (!callExpr) {
        Log::err("Invalid cast to CallExpression");
      }
      return evaluateCallExpression(callExpr, env);
    }

    case NodeType::RETURN_STATEMENT: {
      auto ret = dynamic_cast<ReturnStatement*>(stmt);
      if (!ret) {
        Log::err("Invalid cast to ReturnStatement");
      }

      RuntimeValue* returnValue = nullptr;
      if (ret->value == nullptr) {
        return new ReturnValue(new NullValue());
      }

      return new ReturnValue(evaluate(ret->value, env));
    }

    case NodeType::COMPARISON_EXPRESSION: {
      auto compExpr = dynamic_cast<ComparisonExpression*>(stmt);
      if (!compExpr) {
        Log::err("Invalid cast to ComparisonExpression");
      }
      return evaluateComparisonExpression(compExpr, env);
    }

    case NodeType::IF_STATEMENT : {
      auto ifStmt = dynamic_cast<IfStatement*>(stmt);
      if (!ifStmt) {
        Log::err("Invalid cast to IfStatement");
      }
      return evaluateIfStatement(ifStmt, env);
    }

    default:
      Log::err("This node has not been setup for interpretation: ", stmt->type);
      return new NullValue();
  }
}

RuntimeValue* Interpreter::evaluateIfStatement(IfStatement* ifStmt, Enviroment& env) {
  auto evalCond = evaluate(ifStmt->cond, env);
  bool shouldEvalBody;
  if (evalCond->type == ValueType::BOOLEAN) {
    auto cast = static_cast<BooleanValue*>(evalCond);
    if (!cast) {
      Log::err("Error casting BooleanValue");
    }

    shouldEvalBody = cast->value == 1;

  } else if (evalCond->type == ValueType::NUMBER_VALUE) {
    auto cast = static_cast<NumberValue*>(evalCond);
    if (!cast) {
      Log::err("Error casting NumberValue");
    }

    shouldEvalBody = cast->value != 0;
  } else {
    Log::err("Cannot handle type ", evalCond->type, " in condition");
  }

  if (shouldEvalBody) {
    
    for (auto stmt : ifStmt->body) {
      evaluate(stmt, env);
    }
  }

  return new NullValue();
};

RuntimeValue* Interpreter::evaluateComparisonExpression(ComparisonExpression* comp, Enviroment& env) {
  auto lhs = evaluate(comp->lhs, env);
  auto rhs = evaluate(comp->rhs, env);
  
  bool result = false;

  if (lhs->type == ValueType::NUMBER_VALUE) {
    auto lhsV = static_cast<NumberValue*>(lhs);
    if (!lhsV) {
      Log::err("Error casting NumberValue");
    }
    
    if (rhs->type == ValueType::NUMBER_VALUE) {
      // NUMBER NUMBER
      auto rhsV = static_cast<NumberValue*>(rhs);
      if (!rhsV) {
        Log::err("Error casting NumberValue");
      }
      result = lhsV->value == rhsV->value;
    } else if (rhs->type == ValueType::BOOLEAN) {
      // NUMBER BOOL
      auto rhsV = static_cast<BooleanValue*>(rhs);
      if (!rhsV) {
        Log::err("Error casting BooleanValue");
      }
      if (lhsV->value == 1 && rhsV->value == 1) {
        result = true;
      } else if (lhsV->value == 0 && rhsV->value == 0) {
        result = true;
      }
    }
  } else if (lhs->type == ValueType::NULL_VALUE) {
    auto lhsV = static_cast<NullValue*>(lhs);
    if (!lhsV) {
      Log::err("Error casting NullValue");
    }

    if (rhs->type == ValueType::NULL_VALUE) {
      auto rhsV = static_cast<NullValue*>(rhs);
      if (!rhsV) {
        Log::err("Error casting NullValue");
      }

      result = true;
    }
  } else if (lhs->type == ValueType::STRING_VALUE) {
    auto lhsV = static_cast<StringValue*>(lhs);
    if (!lhsV) {
      Log::err("Error casting StringValue");
    }

    if (rhs->type == ValueType::STRING_VALUE) {
      auto rhsV = static_cast<StringValue*>(rhs);
      if (!rhsV) {
        Log::err("Error casting StringValue");
      }

      result = lhsV->value == rhsV->value;
    }
  } else if (lhs->type == ValueType::BOOLEAN) {
    auto lhsV = static_cast<BooleanValue*>(lhs);
    if (!lhsV) {
      Log::err("Error casting BooleanValue");
    }

    if (rhs->type == ValueType::NUMBER_VALUE) {
      // NUMBER NUMBER
      auto rhsV = static_cast<NumberValue*>(rhs);
      if (!rhsV) {
        Log::err("Error casting NumberValue");
      }
      result = lhsV->value == rhsV->value;
    } else if (rhs->type == ValueType::BOOLEAN) {
      // NUMBER BOOL
      auto rhsV = static_cast<BooleanValue*>(rhs);
      if (!rhsV) {
        Log::err("Error casting BooleanValue");
      }
      if (lhsV->value == 1 && rhsV->value == 1) {
        result = true;
      } else if (lhsV->value == 0 && rhsV->value == 0) {
        result = true;
      }
    }
  } else {
    Log::err("Unrecognized type ", lhs->type, " in comparion");
  }

  // Boolean Null
  // Boolean Number
  // Boolean String
  // Boolean Number
  return new BooleanValue(result);
};

RuntimeValue* Interpreter::evaluateVariableAssignment(VariableAssignment* assign, Enviroment& env) {
  auto value = evaluate(assign->expr, env);

  return env.assignVariable(assign->ident, value);
}

RuntimeValue* Interpreter::evaluateVariableDeclaration(VarDeclaration* decl, Enviroment& env) {
  auto value = evaluate(decl->value, env);
  
  return env.declareVariable(decl->symbol, value, decl->isConstant);
}

RuntimeValue* Interpreter::evaluateIdentifier(Identifier* ident, Enviroment& env) {
  return env.resolve(ident->symbol).variables[ident->symbol]; // Return the value
}

RuntimeValue* Interpreter::evaluateProgram(Program* program, Enviroment& env) {
  RuntimeValue* last = nullptr;

  for (auto stmt : program->body) {
    last = evaluate(stmt, env);
  }

  return last;
}

RuntimeValue* Interpreter::evaluateFunctionDeclaration(FunctionDeclaration* decl, Enviroment& env) {
  auto func = new FunctionValue(decl->name, decl->params, env, decl->body);
  return env.declareVariable(decl->name, func, false);
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

  Log::err("Binary expression not supported for types");
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

RuntimeValue* Interpreter::evaluateCallExpression(CallExpression* expr, Enviroment& env) {
  // Get the function being called
  auto identifier = dynamic_cast<Identifier*>(expr->caller);
  if (!identifier) {
    Log::err("Call expression must be called on an identifier");
  }

  // Resolve the function value
  RuntimeValue* funcVal = env.resolve(identifier->symbol).variables[identifier->symbol];
  if (!funcVal || funcVal->type != ValueType::FUNCTION) {
    Log::err("Attempted to call a non-function: ", identifier->symbol);
  }

  FunctionValue* function = static_cast<FunctionValue*>(funcVal);

  // Check argument count
  if (expr->arguments.size() != function->params.size()) {
    Log::err("Function ", function->name, " expected ", function->params.size(), " arguments, but got ", expr->arguments.size());
  }

  // Evaluate arguments
  std::vector<RuntimeValue*> args;
  for (auto arg : expr->arguments) {
    args.push_back(evaluate(arg, env));
  }

  // Create new function scope
  Enviroment localEnv(&function->env);
  for (size_t i = 0; i < function->params.size(); ++i) {
    localEnv.declareVariable(function->params[i], args[i], false);
  }

  RuntimeValue* returnValue = nullptr;
  for (auto stmt : function->body) {
    RuntimeValue* result = evaluate(stmt, localEnv);

    if (result && result->type == ValueType::RETURN_VALUE) {
      returnValue = static_cast<ReturnValue*>(result)->value;
      break;
    }
  }

  return returnValue ? returnValue : new NullValue();
}