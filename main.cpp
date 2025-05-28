#include "include/log.hpp"
#include "include/lexer.hpp"
#include "include/parser.hpp"
#include "include/interpreter.hpp"
#include "include/enviroment.hpp"
#include <string>

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
  
  Interpreter interpreter = Interpreter();
  
  auto result = interpreter.evaluate(&program, env);
  Log::printValue(result);
}