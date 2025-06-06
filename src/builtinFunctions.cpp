#include "../include/builtinFunctions.hpp"

void declarePrintFunction(Enviroment& env) {
  std::string fnName = "print"; // function name
  
  std::vector<std::string> params = {"toPrint"}; // parameters
  std::vector<Statement* > body; // body

  env.declareVariable(fnName, new FunctionValue(fnName, params, body, [](std::vector<RuntimeValue*> args) -> RuntimeValue* {
    std::string value = "";
    for (auto a : args) {
      if (a->type == ValueType::STRING_VALUE) {
        value += static_cast<StringValue*>(a)->value;
      } else if (a->type == ValueType::NUMBER_VALUE) {
        value += std::to_string(static_cast<NumberValue*>(a)->value);
      } else if (a->type == ValueType::BOOLEAN_VALUE) {
        value += std::to_string(static_cast<BooleanValue*>(a)->value);
      } else if (a->type == ValueType::NULL_VALUE) {
        continue;
      } else {
        Log::err("Unrecognized type ", a->type, " in print function");
      }
    }
    Log::log(value);
    return new NullValue();
  }, env), true);
}


void declareTypeofFunction(Enviroment& env) {
  std::string fnName = "typeof"; // function name
  
  std::vector<std::string> params = {"value"}; // parameters
  std::vector<Statement* > body; // body

  env.declareVariable(fnName, new FunctionValue(fnName, params, body, [](std::vector<RuntimeValue*> args) -> RuntimeValue* {
    RuntimeValue* arg = args[0];
    std::string type = "";
    if (arg->type == ValueType::STRING_VALUE) {
      type = "string";
    } else if (arg->type == ValueType::NUMBER_VALUE) {
      type = "number";
    } else if (arg->type == ValueType::BOOLEAN_VALUE) {
      type = "bool";
    } else if (arg->type == ValueType::NULL_VALUE) {
      type = "null";
    } else if (arg->type == ValueType::FUNCTION_VALUE) {
      type = "function";
    } else {
      Log::err("Unrecognized type ", arg->type, " in 'typeof' function call");
    }
    
    return new StringValue(type);
  }, env), true);
}