#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "values.hpp"
#include "log.hpp"

struct RuntimeValue;

class Enviroment {
  public:
  Enviroment* parent;
  std::unordered_map<std::string, RuntimeValue*> variables;
  std::vector<std::string> constants;

  Enviroment();
  Enviroment(Enviroment* parentEnv);

  RuntimeValue* declareVariable(std::string varname, RuntimeValue* value, bool constant);
  RuntimeValue* assignVariable(std::string varname, RuntimeValue* value);
  Enviroment resolve(std::string& varname);
};