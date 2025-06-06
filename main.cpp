#include "include/log.hpp"
#include "include/lexer.hpp"
#include "include/parser.hpp"
#include "include/interpreter.hpp"
#include "include/enviroment.hpp"
#include "include/builtinFunctions.hpp"
#include <string>
#include <vector>


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

  // DEBUG
  // Log::printAST(program);
  // END DEBUG

  Enviroment env = Enviroment();

  env.declareVariable("x", new NumberValue(1), true);
  declarePrintFunction(env);
  declareTypeofFunction(env);
  
  Interpreter interpreter = Interpreter();
  
  auto result = interpreter.evaluate(&program, env);
  
  // DEBUG
  // if (result) {
  //   Log::printValue(result);
  // }
  // END DEBUG
}