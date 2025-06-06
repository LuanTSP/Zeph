#pragma once
#include "node.hpp"
#include "values.hpp"
#include <functional>

enum ValueType {
  NULL_VALUE,
  NUMBER_VALUE,
  STRING_VALUE,
  RETURN_VALUE,
  BOOLEAN_VALUE,
  FUNCTION_VALUE,
  BREAK_VALUE,
  CONTINUE_VALUE,
};

class Enviroment;

struct RuntimeValue {
  public:
  ValueType type;

  RuntimeValue(ValueType type) : type(type) {} 
};

struct ReturnValue : RuntimeValue {
  public:
  RuntimeValue* value;

  ReturnValue(RuntimeValue* value) : RuntimeValue(ValueType::RETURN_VALUE), value(value) {};
};

struct NullValue : RuntimeValue {
  public:
  std::string value = "null";
  NullValue() : RuntimeValue(ValueType::NULL_VALUE) {};
};

struct NumberValue : RuntimeValue {
  public:
  float value;
  NumberValue(float value) : RuntimeValue(ValueType::NUMBER_VALUE), value(value) {};
};

struct StringValue : RuntimeValue {
  public:
  std::string value;

  StringValue(std::string value) : RuntimeValue(ValueType::STRING_VALUE), value(value) {};
};

struct BooleanValue : RuntimeValue {
  public:
  bool value;
  BooleanValue(bool value) : RuntimeValue(ValueType::BOOLEAN_VALUE), value(value) {};
};

struct FunctionValue : RuntimeValue {
  public:
  std::string name;
  std::vector<std::string> params;
  std::vector<Statement*> body;
  std::function<RuntimeValue* (std::vector<RuntimeValue*> args)> extCall = nullptr;
  Enviroment& env;
  
  FunctionValue(std::string& name, std::vector<std::string>& params, std::vector<Statement*>& body, std::function<RuntimeValue* (std::vector<RuntimeValue*> args)> extCall, Enviroment& env) : RuntimeValue(ValueType::FUNCTION_VALUE), name(name), params(params), body(body), extCall(extCall), env(env) {}
};

struct BreakValue : RuntimeValue {
  public:
  
  BreakValue() : RuntimeValue(ValueType::BREAK_VALUE) {};
};

struct ContinueValue : RuntimeValue {
  public:
  
  ContinueValue() : RuntimeValue(ValueType::CONTINUE_VALUE) {};
};

