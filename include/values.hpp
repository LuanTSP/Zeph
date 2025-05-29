#pragma once
#include "node.hpp"
#include "values.hpp"

enum ValueType {
  NULL_VALUE,
  NUMBER_VALUE,
  STRING_VALUE,
  BOOLEAN,
  FUNCTION,
};

class Enviroment;

struct RuntimeValue {
  public:
  ValueType type;

  RuntimeValue(ValueType type) : type(type) {} 
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
  BooleanValue(bool value) : RuntimeValue(ValueType::BOOLEAN), value(value) {};
};

struct FunctionValue : RuntimeValue {
  public:
  std::string name;
  std::vector<std::string> params;
  Enviroment& env;
  std::vector<Statement*> body;
  
  FunctionValue(std::string& name, std::vector<std::string>& params, Enviroment& env, std::vector<Statement*>& body) : RuntimeValue(ValueType::FUNCTION), name(name), params(params), env(env), body(body) {}
};

