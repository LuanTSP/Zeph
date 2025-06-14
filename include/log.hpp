#pragma once
#include <iostream>
#include "node.hpp"
#include "values.hpp"

class Log {
public:
  template <typename... Args> static void log(Args&&... args) {
    (std::cout << ... << args) << std::endl;
  }

  template <typename... Args> static void err(Args&&... args) {
    std::cerr << "[ERROR]: ";
    (std::cerr << ... << args) << std::endl;
    std::exit(EXIT_FAILURE);
  }

  template <typename... Args> static void info(Args&&... args) {
    std::cout << "[INFO]: ";
    (std::cout << ... << args) << std::endl;
  }

  static void printAST(const Statement* node, int indent = 0) {
    if (!node) return;

    auto printIndent = [indent](int offset = 0) {
      for (int i = 0; i < indent + offset; ++i) std::cout << "   ";
    };

    switch (node->type) {
      case NodeType::PROGRAM: {
        auto program = dynamic_cast<const Program*>(node);
        if (!program) {
          err("Invalid PROGRAM node cast.");
          return;
        }
        printIndent();
        log("Program: {");
        for (const auto& stmt : program->body) {
          printAST(stmt, indent + 1);
        }
        printIndent();
        log("}");
        break;
      }

      case NodeType::VAR_DECLARATION: {
        auto declaration = dynamic_cast<const VarDeclaration*>(node);
        if (!declaration) {
          err("Invalid VAR_DECLARATION node cast.");
          return;
        }
        printIndent();
        log("VarDeclaration: ", declaration->symbol, " {");
        printIndent(1); log("symbol: {", declaration->symbol, "}");
        printIndent(1); log("constant: {", declaration->isConstant, "}");
        printAST(declaration->value, indent + 1);
        printIndent();
        log("}");
        break;
      }

      case NodeType::FUNC_DECLARATION: {
        auto declaration = dynamic_cast<const FunctionDeclaration*>(node);
        if (!declaration) {
          err("Invalid FUNC_DECLARATION node cast.");
          return;
        }
        printIndent();
        log("FuncDeclaration: ", declaration->name, " {");

        std::string pStr = "params: {";
        for (size_t i = 0; i < declaration->params.size(); ++i) {
          pStr += declaration->params[i];
          if (i != declaration->params.size() - 1) pStr += ", ";
        }
        pStr += "}";

        printIndent(1); log(pStr);
        printIndent(1); log("body: {");
        for (const auto& stmt : declaration->body) {
          printAST(stmt, indent + 2);
        }
        printIndent(1); log("}");
        printIndent(); log("}");
        break;
      }

      case NodeType::IDENTIFIER_LITERAL: {
        auto ident = dynamic_cast<const Identifier*>(node);
        if (!ident) {
          err("Invalid IDENTIFIER_LITERAL node cast.");
          return;
        }
        printIndent();
        log("Identifier: {", ident->symbol, "}");
        break;
      }

      case NodeType::NUMERIC_LITERAL: {
        auto num = dynamic_cast<const NumericLiteral*>(node);
        if (!num) {
          err("Invalid NUMERIC_LITERAL node cast.");
          return;
        }
        printIndent();
        log("NumericLiteral: {", num->value, "}");
        break;
      }

      case NodeType::STRING_LITERAL: {
        auto str = dynamic_cast<const StringLiteral*>(node);
        if (!str) {
          err("Invalid STRING_LITERAL node cast.");
          return;
        }
        printIndent();
        log("StringLiteral: {\"", str->value, "\"}");
        break;
      }

      case NodeType::NULL_LITERAL: {
        auto nll = dynamic_cast<const NullLiteral*>(node);
        if (!nll) {
          err("Invalid NULL_LITERAL node cast");
          return;
        }
        printIndent();
        log("NullLiteral: {", nll->value, "}");
        break;
      }

      case NodeType::BOOLEAN_LITERAL: {
        auto bll = dynamic_cast<const BooleanLiteral*>(node);
        if (!bll) {
          err("Invalid BOOLEAN_LITERAL node cast");
          return;
        }
        printIndent();
        log("BooleanLiteral: {", bll->value, "}");
        break;
      }

      case NodeType::BINARY_EXPRESSION: {
        auto bin = dynamic_cast<const BinaryExpression*>(node);
        if (!bin) {
          err("Invalid BINARY_EXPRESSION node cast.");
          return;
        }
        printIndent();
        log("BinaryExpression: ", bin->op, " {");
        printAST(bin->left, indent + 1);
        printAST(bin->right, indent + 1);
        printIndent();
        log("}");
        break;
      }

      case NodeType::VAR_ASSIGNMENT: {
        auto assign = dynamic_cast<const VariableAssignment*>(node);
        if (!assign) {
          err("Invalid VariableAssignment node cast.");
          return;
        }
        printIndent();
        log("VaribleAssignment: ", assign->ident, " {");
        printAST(assign->expr, indent + 1);
        printIndent();
        log("}");
        break;
      }

      case NodeType::COMPARISON_EXPRESSION: {
        auto comp = dynamic_cast<const ComparisonExpression*>(node);
        if (!comp) {
          err("Invalid ComparisonExpression node cast.");
          return;
        }
        printIndent();
        log("ComparisonExpression: == {");
        printAST(comp->lhs, indent + 1);
        printAST(comp->rhs, indent + 1);
        printIndent();
        log("}");
        break;
      }

      case NodeType::LOGICAL_EXPRESSION: {
        auto logic = dynamic_cast<const LogicalExpression*>(node);
        if (!logic) {
          err("Invalid LogicalExpression node cast.");
          return;
        }
        printIndent();
        log("Logical Expression: ", logic->op, " {");
        printAST(logic->lhs, indent + 1);
        printAST(logic->rhs, indent + 1);
        printIndent();
        log("}");
        break;
      }

      default:
        printIndent();
        log("Unknown Node Type");
        break;
    }
  }

  static void printAST(const Program& program) {
    log("\nAST");
    printAST(&program);
  }

  static void printValue(RuntimeValue* program) {
    if (program->type == ValueType::NUMBER_VALUE) {
      auto cast = static_cast<NumberValue*>(program);
      if (!cast) {
        Log::err("Error casting");
      }
      log("Result: {", cast->value, "}");
    } else if (program->type == ValueType::STRING_VALUE) {
      auto cast = static_cast<StringValue*>(program);
      if (!cast) {
        Log::err("Error casting");
      }
      log("Result: {\"", cast->value, "\"}");
    } else if (program->type == ValueType::NULL_VALUE) {
      auto cast = static_cast<NullValue*>(program);
      if (!cast) {
        Log::err("Error casting");
      }
      log("Result: {", cast->value, "}");
    } else if (program->type == ValueType::BOOLEAN_VALUE) {
      auto cast = static_cast<BooleanValue*>(program);
      if (!cast) {
        Log::err("Error casting");
      }
      log("Result: {", cast->value, "}");
    } else if (program->type == ValueType::FUNCTION_VALUE) {
      auto cast = static_cast<FunctionValue*>(program);
      if (!cast) {
        Log::err("Error casting");
      }
      log("Result: {function: ", cast->name, "}");
    } else if (program->type == ValueType::RETURN_VALUE) {
      auto cast = static_cast<ReturnValue*>(program);
      if (!cast) {
        Log::err("Error casting");
      }
      
      if (cast->value->type == ValueType::NUMBER_VALUE) {
        auto result = static_cast<NumberValue*>(cast->value);
        if (!result) {
          Log::err("Error casting");
        }
        log("Result: {", result->value, "}");
      } else if (cast->value->type == ValueType::BOOLEAN_VALUE) {
        auto result = static_cast<BooleanValue*>(cast->value);
        if (!result) {
          Log::err("Error casting");
        }
        log("Result: {", result->value, "}");
      } else if (cast->value->type == ValueType::STRING_VALUE) {
        auto result = static_cast<StringValue*>(cast->value);
        if (!result) {
          Log::err("Error casting");
        }
        log("Result: {\"", result->value, "\"}");
      } else if (cast->value->type == ValueType::NULL_VALUE) {
        auto result = static_cast<NullValue*>(cast->value);
        if (!result) {
          Log::err("Error casting");
        }
        log("Result: {", result->value, "}");
      }
    } else {
      log("Unrecognized type in program result: ", program->type);
    }
  }
};
