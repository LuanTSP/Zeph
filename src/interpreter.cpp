#include "../include/interpreter.hpp"
#include <string>
#include <format>

Interpreter::Interpreter() {};

RuntimeValue *Interpreter::evaluate(Statement *stmt, Enviroment &env) {
  switch (stmt->type) {
  case NodeType::PROGRAM: {
    auto program = dynamic_cast<Program *>(stmt);
    if (!program) {
      Log::err("Invalid cast to Program");
    }
    return evaluateProgram(program, env);
  }

  case NodeType::NUMERIC_LITERAL: {
    auto nLiteral = dynamic_cast<NumericLiteral *>(stmt);
    if (!nLiteral) {
      Log::err("Invalid cast to NumericLiteral");
    }
    
    return new NumberValue(std::stof(nLiteral->value));
  }

  case NodeType::IDENTIFIER_LITERAL: {
    auto ident = dynamic_cast<Identifier *>(stmt);
    if (!ident) {
      Log::err("Invalid cast to Identifier");
    }

    return evaluateIdentifier(ident, env);
  }

  case NodeType::STRING_LITERAL: {
    auto str = dynamic_cast<StringLiteral *>(stmt);
    if (!str) {
      Log::err("Invalid cast to StringLiteral");
    }

    return new StringValue(str->value);
  }

  case NodeType::NULL_LITERAL: {
    auto nll = dynamic_cast<NullLiteral *>(stmt);
    if (!nll) {
      Log::err("Invalid cast to NullLiteral");
    }

    return new NullValue();
  }

  case NodeType::BOOLEAN_LITERAL: {
    auto bll = dynamic_cast<BooleanLiteral *>(stmt);
    if (!bll) {
      Log::err("Invalid cast to BooleanLiteral");
    }

    bool value = bll->value == "true" ? 1 : 0;
    return new BooleanValue(value);
  }

  case NodeType::VAR_DECLARATION: {
    auto decl = dynamic_cast<VarDeclaration *>(stmt);
    if (!decl) {
      Log::err("Invalid cast to VarDeclaration");
    }

    return evaluateVariableDeclaration(decl, env);
  }

  case NodeType::VAR_ASSIGNMENT: {
    auto assign = dynamic_cast<VariableAssignment *>(stmt);
    if (!assign) {
      Log::err("Invalid cast to VariableAssignment");
    }

    return evaluateVariableAssignment(assign, env);
  }

  case NodeType::BINARY_EXPRESSION: {
    auto binExpr = dynamic_cast<BinaryExpression *>(stmt);
    if (!binExpr) {
      Log::err("Invalid cast to BinaryExpression");
    }
    return evaluateBinaryExpression(binExpr, env);
  }

  case NodeType::FUNC_DECLARATION: {
    auto fDecl = dynamic_cast<FunctionDeclaration *>(stmt);
    if (!fDecl) {
      Log::err("Invalid cast to FunctionDeclaration");
    }
    return evaluateFunctionDeclaration(fDecl, env);
  }

  case NodeType::CALL_EXPRESSION: {
    auto callExpr = dynamic_cast<CallExpression *>(stmt);
    if (!callExpr) {
      Log::err("Invalid cast to CallExpression");
    }
    return evaluateCallExpression(callExpr, env);
  }

  case NodeType::RETURN_STATEMENT: {
    auto ret = dynamic_cast<ReturnStatement *>(stmt);
    if (!ret) {
      Log::err("Invalid cast to ReturnStatement");
    }

    RuntimeValue *returnValue = nullptr;
    if (ret->value == nullptr) {
      return new ReturnValue(new NullValue());
    }

    return new ReturnValue(evaluate(ret->value, env));
  }

  case NodeType::COMPARISON_EXPRESSION: {
    auto compExpr = dynamic_cast<ComparisonExpression *>(stmt);
    if (!compExpr) {
      Log::err("Invalid cast to ComparisonExpression");
    }
    return evaluateComparisonExpression(compExpr, env);
  }

  case NodeType::IF_STATEMENT: {
    auto ifStmt = dynamic_cast<IfStatement *>(stmt);
    if (!ifStmt) {
      Log::err("Invalid cast to IfStatement");
    }
    return evaluateIfStatement(ifStmt, env);
  }

  case NodeType::WHILE_STATEMENT: {
    auto whileStmt = dynamic_cast<WhileStatement *>(stmt);
    if (!whileStmt) {
      Log::err("Invalid cast to WhileStatement");
    }
    return evaluateWhileStatement(whileStmt, env);
  }

  case NodeType::BREAK_STATEMENT: {
    auto breakStmt = dynamic_cast<BreakStatement *>(stmt);
    if (!breakStmt) {
      Log::err("Invalid cast to BreakStatement");
    }
    return new BreakValue();
  }

  case NodeType::CONTINUE_STATEMENT: {
    auto contStmt = dynamic_cast<ContinueStatement *>(stmt);
    if (!contStmt) {
      Log::err("Invalid cast to ContinueStatement");
    }
    return new ContinueValue();
  }

  case NodeType::LOGICAL_EXPRESSION: {
    auto logiExp = dynamic_cast<LogicalExpression *>(stmt);
    if (!logiExp) {
      Log::err("Invalid cast to LogicalExpression");
    }
    return evaluateLogicalExpression(logiExp, env);
  }

  default:
    Log::err("This node has not been setup for interpretation: ", stmt->type);
    return new NullValue();
  }
}

bool evaluateLogicalExpressionNumeric(bool a, bool b, std::string &op) {
  if (op == "or") {
    return a || b;
  } else if (op == "and") {
    return a && b;
  } else {
    Log::err("Unrecognized logical operator ", op);
    return false; // unrecheable
  }
}

RuntimeValue *Interpreter::evaluateLogicalExpression(LogicalExpression *logic,
                                                     Enviroment &env) {
  auto lhsV = evaluate(logic->lhs, env);
  auto rhsV = evaluate(logic->rhs, env);

  bool result;
  if (lhsV->type == ValueType::BOOLEAN_VALUE &&
      rhsV->type == ValueType::BOOLEAN_VALUE) {
    result = evaluateLogicalExpressionNumeric(
        static_cast<BooleanValue *>(lhsV)->value,
        static_cast<BooleanValue *>(rhsV)->value, logic->op);
  } else if (lhsV->type == ValueType::NUMBER_VALUE &&
             rhsV->type == ValueType::NUMBER_VALUE) {
    bool a = false;
    bool b = false;

    if (static_cast<NumberValue *>(lhsV)->value == 1)
      a = true;
    if (static_cast<NumberValue *>(rhsV)->value == 1)
      b = true;

    result = evaluateLogicalExpressionNumeric(a, b, logic->op);
  } else if (lhsV->type == ValueType::NUMBER_VALUE &&
             rhsV->type == ValueType::BOOLEAN_VALUE) {
    bool a = false;
    bool b = static_cast<NumberValue *>(rhsV)->value;

    if (static_cast<NumberValue *>(lhsV)->value == 1)
      a = true;

    result = evaluateLogicalExpressionNumeric(a, b, logic->op);
  } else if (lhsV->type == ValueType::BOOLEAN_VALUE &&
             rhsV->type == ValueType::NUMBER_VALUE) {
    bool a = static_cast<NumberValue *>(lhsV)->value;
    bool b = false;

    if (static_cast<NumberValue *>(rhsV)->value == 1)
      b = true;

    result = evaluateLogicalExpressionNumeric(a, b, logic->op);
  } else {
    Log::err("Unsupported logical operation between ", lhsV->type, " and ",
             rhsV->type);
  }

  return new BooleanValue(result);
}

RuntimeValue *Interpreter::evaluateWhileStatement(WhileStatement *whileStmt,
                                                  Enviroment &env) {
  auto evalCond = evaluate(whileStmt->cond, env);
  bool shouldEvalBody;

  if (evalCond->type == ValueType::BOOLEAN_VALUE) {
    auto cast = static_cast<BooleanValue *>(evalCond);
    if (!cast) {
      Log::err("Error casting BooleanValue");
    }

    shouldEvalBody = cast->value == 1;

  } else if (evalCond->type == ValueType::NUMBER_VALUE) {
    auto cast = static_cast<NumberValue *>(evalCond);
    if (!cast) {
      Log::err("Error casting NumberValue");
    }

    shouldEvalBody = cast->value != 0;
  } else if (evalCond->type == ValueType::NULL_VALUE) {
    shouldEvalBody = false;
  } else {
    Log::err("Cannot handle type ", evalCond->type, " in condition");
  }

  RuntimeValue *toReturn = nullptr;
  bool breakLoop = false;

  while (shouldEvalBody) {
    for (auto stmt : whileStmt->body) {
      auto value = evaluate(stmt, env);

      if (value->type == ValueType::RETURN_VALUE) {
        toReturn = static_cast<ReturnValue *>(value);
        breakLoop = true;
        break;
      } else if (value->type == ValueType::BREAK_VALUE) {
        breakLoop = true;
        break;
      } else if (value->type == ValueType::CONTINUE_VALUE) {
        // do not break the loop
        break;
      }
    }

    if (breakLoop) {
      break;
    }

    evalCond = evaluate(whileStmt->cond, env);

    if (evalCond->type == ValueType::BOOLEAN_VALUE) {
      auto cast = static_cast<BooleanValue *>(evalCond);
      if (!cast) {
        Log::err("Error casting BooleanValue");
      }

      shouldEvalBody = cast->value == 1;

    } else if (evalCond->type == ValueType::NUMBER_VALUE) {
      auto cast = static_cast<NumberValue *>(evalCond);
      if (!cast) {
        Log::err("Error casting NumberValue");
      }

      shouldEvalBody = cast->value != 0;
    } else {
      Log::err("Cannot handle type ", evalCond->type, " in condition");
    }
  }

  return toReturn ? toReturn : new NullValue();
}

RuntimeValue *Interpreter::evaluateIfStatement(IfStatement *ifStmt,
                                               Enviroment &env) {
  auto evalCond = evaluate(ifStmt->cond, env);
  bool shouldEvalBody;
  if (evalCond->type == ValueType::BOOLEAN_VALUE) {
    auto cast = static_cast<BooleanValue *>(evalCond);
    if (!cast) {
      Log::err("Error casting BooleanValue");
    }

    shouldEvalBody = cast->value == 1;

  } else if (evalCond->type == ValueType::NUMBER_VALUE) {
    auto cast = static_cast<NumberValue *>(evalCond);
    if (!cast) {
      Log::err("Error casting NumberValue");
    }

    shouldEvalBody = cast->value != 0;
  } else if (evalCond->type == ValueType::NULL_VALUE) {
    shouldEvalBody = false;
  } else {
    Log::err("Cannot handle type ", evalCond->type, " in condition");
  }

  RuntimeValue *toReturn = nullptr;

  if (shouldEvalBody) {
    for (auto stmt : ifStmt->ifBody) {
      auto value = evaluate(stmt, env);

      if (value->type == ValueType::RETURN_VALUE) {
        toReturn = static_cast<ReturnValue *>(value);
        break;
      } else if (value->type == ValueType::BREAK_VALUE) {
        toReturn = static_cast<BreakValue *>(value);
        break;
      } else if (value->type == ValueType::CONTINUE_VALUE) {
        toReturn = static_cast<ContinueValue *>(value);
        break;
      }
    }
  } else {
    for (auto stmt : ifStmt->elseBody) {
      auto value = evaluate(stmt, env);

      if (value->type == ValueType::RETURN_VALUE) {
        toReturn = static_cast<ReturnValue *>(value);
        break;
      } else if (value->type == ValueType::BREAK_VALUE) {
        toReturn = static_cast<BreakValue *>(value);
        break;
      } else if (value->type == ValueType::CONTINUE_VALUE) {
        toReturn = static_cast<ContinueValue *>(value);
        break;
      }
    }
  }

  return toReturn ? toReturn : new NullValue();
};

RuntimeValue *
Interpreter::evaluateComparisonExpression(ComparisonExpression *comp,
                                          Enviroment &env) {
  bool result = calculateComparizon(comp, env);

  return new BooleanValue(result);
};

bool Interpreter::calculateComparizon(ComparisonExpression *comp,
                                      Enviroment &env) {
  RuntimeValue *lhs = evaluate(comp->lhs, env);
  RuntimeValue *rhs = evaluate(comp->rhs, env);
  std::string op = comp->op;

  bool result = false;

  if (op == "==") {
    if (lhs->type == ValueType::NUMBER_VALUE) {
      auto lhsV = static_cast<NumberValue *>(lhs);
      if (!lhsV) {
        Log::err("Error casting NumberValue");
      }

      if (rhs->type == ValueType::NUMBER_VALUE) {
        // NUMBER NUMBER
        auto rhsV = static_cast<NumberValue *>(rhs);
        if (!rhsV) {
          Log::err("Error casting NumberValue");
        }
        result = lhsV->value == rhsV->value;
      } else if (rhs->type == ValueType::BOOLEAN_VALUE) {
        // NUMBER BOOL
        auto rhsV = static_cast<BooleanValue *>(rhs);
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
      auto lhsV = static_cast<NullValue *>(lhs);
      if (!lhsV) {
        Log::err("Error casting NullValue");
      }

      if (rhs->type == ValueType::NULL_VALUE) {
        auto rhsV = static_cast<NullValue *>(rhs);
        if (!rhsV) {
          Log::err("Error casting NullValue");
        }

        result = true;
      }
    } else if (lhs->type == ValueType::STRING_VALUE) {
      auto lhsV = static_cast<StringValue *>(lhs);
      if (!lhsV) {
        Log::err("Error casting StringValue");
      }

      if (rhs->type == ValueType::STRING_VALUE) {
        auto rhsV = static_cast<StringValue *>(rhs);
        if (!rhsV) {
          Log::err("Error casting StringValue");
        }

        result = lhsV->value == rhsV->value;
      }
    } else if (lhs->type == ValueType::BOOLEAN_VALUE) {
      auto lhsV = static_cast<BooleanValue *>(lhs);
      if (!lhsV) {
        Log::err("Error casting BooleanValue");
      }

      if (rhs->type == ValueType::NUMBER_VALUE) {
        // NUMBER NUMBER
        auto rhsV = static_cast<NumberValue *>(rhs);
        if (!rhsV) {
          Log::err("Error casting NumberValue");
        }
        result = lhsV->value == rhsV->value;
      } else if (rhs->type == ValueType::BOOLEAN_VALUE) {
        // NUMBER BOOL
        auto rhsV = static_cast<BooleanValue *>(rhs);
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
  } else if (op == "<" || op == "<=" || op == ">" ||
             op == ">=") { // only support number x bool
    if (lhs->type == ValueType::NUMBER_VALUE) {
      auto lhsV = static_cast<NumberValue *>(lhs);
      if (!lhsV) {
        Log::err("Error casting NumberValue");
      }

      if (rhs->type == ValueType::NUMBER_VALUE) {
        // NUMBER NUMBER
        auto rhsV = static_cast<NumberValue *>(rhs);
        if (!rhsV) {
          Log::err("Error casting NumberValue");
        }

        if (op == "<")
          return lhsV->value < rhsV->value;
        else if (op == "<=")
          return lhsV->value <= rhsV->value;
        else if (op == ">")
          return lhsV->value > rhsV->value;
        else if (op == ">=")
          return lhsV->value >= rhsV->value;
        else {
          Log::err("Unrecognized operator ", op);
        }
      } else if (rhs->type == ValueType::BOOLEAN_VALUE) {
        // NUMBER BOOL
        auto rhsV = static_cast<BooleanValue *>(rhs);
        if (!rhsV) {
          Log::err("Error casting BooleanValue");
        }

        if (op == "<")
          return lhsV->value < rhsV->value;
        else if (op == "<=")
          return lhsV->value <= rhsV->value;
        else if (op == ">")
          return lhsV->value > rhsV->value;
        else if (op == ">=")
          return lhsV->value >= rhsV->value;
        else {
          Log::err("Unrecognized operator ", op);
        }
      }
    } else if (lhs->type == ValueType::BOOLEAN_VALUE) {
      auto lhsV = static_cast<BooleanValue *>(lhs);
      if (!lhsV) {
        Log::err("Error casting BooleanValue");
      }

      if (rhs->type == ValueType::NUMBER_VALUE) {
        // NUMBER NUMBER
        auto rhsV = static_cast<NumberValue *>(rhs);
        if (!rhsV) {
          Log::err("Error casting NumberValue");
        }

        if (op == "<")
          return lhsV->value < rhsV->value;
        else if (op == "<=")
          return lhsV->value <= rhsV->value;
        else if (op == ">")
          return lhsV->value > rhsV->value;
        else if (op == ">=")
          return lhsV->value >= rhsV->value;
        else {
          Log::err("Unrecognized operator ", op);
        }
      } else if (rhs->type == ValueType::BOOLEAN_VALUE) {
        // NUMBER BOOL
        auto rhsV = static_cast<BooleanValue *>(rhs);
        if (!rhsV) {
          Log::err("Error casting BooleanValue");
        }

        if (op == "<")
          return lhsV->value < rhsV->value;
        else if (op == "<=")
          return lhsV->value <= rhsV->value;
        else if (op == ">")
          return lhsV->value > rhsV->value;
        else if (op == ">=")
          return lhsV->value >= rhsV->value;
        else {
          Log::err("Unrecognized operator ", op);
        }
      }
    } else {
      Log::err("Unrecognized type ", lhs->type, " in comparion");
    }
  } else {
    Log::err("Unrecognized comparison operator \"", op, "\"");
  }

  return result;
};

RuntimeValue *
Interpreter::evaluateVariableAssignment(VariableAssignment *assign,
                                        Enviroment &env) {
  auto value = evaluate(assign->expr, env);

  return env.assignVariable(assign->ident, value);
}

RuntimeValue *Interpreter::evaluateVariableDeclaration(VarDeclaration *decl,
                                                       Enviroment &env) {
  auto value = evaluate(decl->value, env);

  return env.declareVariable(decl->symbol, value, decl->isConstant);
}

RuntimeValue *Interpreter::evaluateIdentifier(Identifier *ident,
                                              Enviroment &env) {
  return env.resolve(ident->symbol)
      .variables[ident->symbol]; // Return the value
}

RuntimeValue *Interpreter::evaluateProgram(Program *program, Enviroment &env) {
  RuntimeValue *last = nullptr;

  for (auto stmt : program->body) {
    last = evaluate(stmt, env);
  }

  return last;
}

RuntimeValue *
Interpreter::evaluateFunctionDeclaration(FunctionDeclaration *decl,
                                         Enviroment &env) {
  auto func =
      new FunctionValue(decl->name, decl->params, decl->body, nullptr, env);
  return env.declareVariable(decl->name, func, false);
}

RuntimeValue *Interpreter::evaluateBinaryExpression(BinaryExpression *binExpr,
                                                    Enviroment &env) {
  RuntimeValue *left = evaluate(binExpr->left, env);
  RuntimeValue *right = evaluate(binExpr->right, env);

  if (left->type == ValueType::NUMBER_VALUE &&
      right->type == ValueType::NUMBER_VALUE) {
    float result = calculateNumericBinaryExpression(
        static_cast<NumberValue *>(left)->value,
        static_cast<NumberValue *>(right)->value, binExpr->op);

    return new NumberValue(result);

  } else if (left->type == ValueType::BOOLEAN_VALUE &&
             right->type == ValueType::BOOLEAN_VALUE) {
    bool bLeft = static_cast<BooleanValue *>(left)->value;
    bool bRight = static_cast<BooleanValue *>(right)->value;

    return new NumberValue(bLeft + bRight);

  } else if (left->type == ValueType::BOOLEAN_VALUE &&
             right->type == ValueType::NUMBER_VALUE) {
    bool bLeft = static_cast<BooleanValue *>(left)->value;
    float nRight = static_cast<NumberValue *>(right)->value;

    return new NumberValue(bLeft + nRight);

  } else if (left->type == ValueType::NUMBER_VALUE &&
             right->type == ValueType::BOOLEAN_VALUE) {
    float nLeft = static_cast<NumberValue *>(left)->value;
    bool bRight = static_cast<BooleanValue *>(right)->value;

    return new NumberValue(nLeft + bRight);

  } else if (left->type == ValueType::STRING_VALUE &&
             right->type == ValueType::STRING_VALUE) {
    std::string sLeft = static_cast<StringValue *>(left)->value;
    std::string sRight = static_cast<StringValue *>(right)->value;
    return new StringValue(sLeft + sRight);

  } else if (left->type == ValueType::STRING_VALUE && right->type == ValueType::NUMBER_VALUE) {
    std::string sLeft = static_cast<StringValue *>(left)->value;
    std::string sRight = std::to_string(static_cast<NumberValue *>(right)->value);  
    return new StringValue(sLeft + sRight);

  } else if (left->type == ValueType::NUMBER_VALUE && right->type == ValueType::STRING_VALUE) {
    std::string sLeft = std::to_string(static_cast<NumberValue *>(left)->value);
    std::string sRight = static_cast<StringValue *>(right)->value;  
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

float Interpreter::calculateNumericBinaryExpression(float left, float right,
                                                    const std::string &op) {
  if (op == "+")
    return left + right;
  else if (op == "-")
    return left - right;
  else if (op == "*")
    return left * right;
  else if (op == "/") {
    if (right == 0) {
      Log::err("Division by zero");
    }

    return left / right;
  } else if (op == "%") {
    if (right == 0) {
      Log::err("Division by zero");
    }
    // Get only the integer part
    return (int)left % (int)right;
  }

  Log::err("Unknown numeric operator: " + op);
  return 0.0f;
}

RuntimeValue *Interpreter::evaluateCallExpression(CallExpression *expr,
                                                  Enviroment &env) {
  // Get the function being called
  auto identifier = dynamic_cast<Identifier *>(expr->caller);
  if (!identifier) {
    Log::err("Call expression must be called on an identifier");
  }

  // Resolve the function value
  RuntimeValue *funcVal =
      env.resolve(identifier->symbol).variables[identifier->symbol];
  if (!funcVal || funcVal->type != ValueType::FUNCTION_VALUE) {
    Log::err("Attempted to call a non-function: ", identifier->symbol);
  }

  FunctionValue *function = static_cast<FunctionValue *>(funcVal);

  // Check argument count
  if (expr->arguments.size() != function->params.size()) {
    Log::err("Function ", function->name, " expected ", function->params.size(),
             " arguments, but got ", expr->arguments.size());
  }

  // Evaluate arguments
  std::vector<RuntimeValue *> args;
  for (auto arg : expr->arguments) {
    args.push_back(evaluate(arg, env));
  }

  // Return early if extCall is defined and returns a runtime value that is not
  // null
  if (function->extCall != nullptr) {
    auto value = function->extCall(args);
    if (value->type == ValueType::STRING_VALUE) {
      auto cast = static_cast<StringValue *>(value);
      return new StringValue(cast->value);
    } else if (value->type == ValueType::NULL_VALUE) {
      return new NullValue();
    } else if (value->type == ValueType::NUMBER_VALUE) {
      auto cast = static_cast<NumberValue *>(value);
      return new NumberValue(cast->value);
    } else if (value->type == ValueType::BOOLEAN_VALUE) {
      auto cast = static_cast<BooleanValue *>(value);
      return new BooleanValue(cast->value);
    } else {
      Log::err("Unsupported value type ", value->type, " returned by extnCall");
    }
  }

  // Create new function scope
  Enviroment localEnv(&function->env);
  for (size_t i = 0; i < function->params.size(); ++i) {
    localEnv.declareVariable(function->params[i], args[i], false);
  }

  RuntimeValue *returnValue = nullptr;
  for (auto stmt : function->body) {
    RuntimeValue *result = evaluate(stmt, localEnv);

    if (result && result->type == ValueType::RETURN_VALUE) {
      returnValue = static_cast<ReturnValue *>(result)->value;
      break;
    }
  }

  // Run external call
  if (function->extCall) {
    function->extCall(args);
  }

  return returnValue ? returnValue : new NullValue();
}
