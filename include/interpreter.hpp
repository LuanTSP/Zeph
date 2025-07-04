#pragma once
#include "values.hpp"
#include "enviroment.hpp"
#include "node.hpp"
#include "log.hpp"
#include <string>

class Interpreter {
public:
  Interpreter();

  RuntimeValue* evaluate(Statement* stmt, Enviroment& env);
  RuntimeValue* evaluateBinaryExpression(BinaryExpression* binExpr, Enviroment& env);
  RuntimeValue* evaluateProgram(Program* program, Enviroment& env);
  RuntimeValue* evaluateIdentifier(Identifier* ident, Enviroment& env);
  RuntimeValue* evaluateVariableDeclaration(VarDeclaration* decl, Enviroment& env);
  RuntimeValue* evaluateFunctionDeclaration(FunctionDeclaration* decl, Enviroment& env);
  RuntimeValue* evaluateCallExpression(CallExpression* expr, Enviroment& env);
  RuntimeValue* evaluateComparisonExpression(ComparisonExpression* comp, Enviroment& env);
  RuntimeValue* evaluateLogicalExpression(LogicalExpression* logic, Enviroment& env);
  RuntimeValue* evaluateVariableAssignment(VariableAssignment* assign, Enviroment& env);
  RuntimeValue* evaluateIfStatement(IfStatement* ifStmt, Enviroment& env);
  RuntimeValue* evaluateWhileStatement(WhileStatement* whileStmt, Enviroment& env);
  bool calculateComparizon(ComparisonExpression* comp, Enviroment& env);
  float calculateNumericBinaryExpression(float leftValue, float rightValue, const std::string& op);
};