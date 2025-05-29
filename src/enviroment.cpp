#include "../include/enviroment.hpp"

Enviroment::Enviroment() {
  this->parent = nullptr;
  this->variables = std::unordered_map<std::string, RuntimeValue*>();
  this->constants = std::vector<std::string>();
}

Enviroment::Enviroment(Enviroment* parentEnv) {
  this->parent = parentEnv;
  this->variables = std::unordered_map<std::string, RuntimeValue*>();
  this->constants = std::vector<std::string>();
}

RuntimeValue* Enviroment::declareVariable(std::string varname, RuntimeValue* value, bool constant) {
  if (variables.find(varname) == variables.end()) {
    Log::err("Cannot declare variable", varname, " as it was already declared");
  }

  variables[varname] = value;

  if (constant) {
    constants.push_back(varname);
  }

  return value;
};


RuntimeValue* Enviroment::assignVariable(std::string varname, RuntimeValue* value) {
  Enviroment env = resolve(varname);
  
  for (auto c : env.constants) {
    if (c == varname) {
      Log::err("Cannot reassign variable ", varname, " as it is constant");
    }
  }

  env.variables[varname] = value;
  return value;
};

Enviroment Enviroment::resolve(std::string& varname) {
  if (variables.find(varname) != variables.end()) {
    return *this;
  }

  if (parent == nullptr) {
    std::string errMsg;
    Log::err("Variable ", varname, "does not exist");
  }

  return parent->resolve(varname);
}