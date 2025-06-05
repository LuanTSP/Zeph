#include "include/log.hpp"
#include "include/lexer.hpp"
#include "include/parser.hpp"
#include "include/interpreter.hpp"
#include "include/enviroment.hpp"
#include <string>
#include <vector>

void declarePrintFunction(Enviroment& env) {
  std::string fnName = "print"; // function name
  
  std::vector<std::string> params = {"toPrint"}; // parameters
  std::vector<Statement* > body; // body

  env.declareVariable(fnName, new FunctionValue(fnName, params, body, [](std::vector<RuntimeValue*> args) {
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
  }, env), true);
}

int main(int argc, char* argv[]) {
  // Get filepath
  if (argc == 1) {
    Log::err("At least one file should be given");
  }
  if (argc != 2) {
    Log::err("posea takes only one argument");
  }

  std::string filepath = argv[1];

  Parser parser = Parser();
  Program program = parser.parse(filepath);

  Log::printAST(program);
  
  Enviroment env = Enviroment();

  env.declareVariable("x", new NumberValue(1), true);
  declarePrintFunction(env);
  
  Interpreter interpreter = Interpreter();
  
  auto result = interpreter.evaluate(&program, env);
  if (result) {
    Log::printValue(result);
  }
}