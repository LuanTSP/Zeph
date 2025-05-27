#pragma once
#include <iostream>
#include "node.hpp"

class Log {
public:
  // Logging
  template <typename... Args> static void log(Args &&...args) {
    (std::cout << ... << args) << std::endl;
  }

  template <typename... Args> static void err(Args &&...args) {
    std::cerr << "[ERROR]: ";
    (std::cerr << ... << args) << std::endl;
    std::exit(EXIT_FAILURE);
  }

  template <typename... Args> static void info(Args &&...args) {
    std::cout << "[INFO]: ";
    (std::cout << ... << args) << std::endl;
  }

  static void printAST(const Statement* node, int indent = 0) {
    if (!node) return;

    auto printIndent = [indent]() {
      for (int i = 0; i < indent; ++i) std::cout << "   ";
    };

    switch (node->type) {
      case NodeType::PROGRAM: {
        auto program = dynamic_cast<const Program*>(node);
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
        printIndent();
        log("VarDeclaration: ", declaration->symbol, " {");
        printIndent(); printIndent();
        log("symbol: {", declaration->symbol, "}");
        printIndent(); printIndent();
        log("constant: {", declaration->isConstant, "}");
        printAST(declaration->value, indent + 1);
        printIndent();
        log("}");
        break;
      }
      case NodeType::FUNC_DECLARATION: {
        auto declaration = dynamic_cast<const FunctionDeclaration*>(node);
        printIndent();
        log("FuncDeclaration: ", declaration->name, " {");
        std::string pStr = "params: {";
        for (auto p : declaration->params) {
          pStr += p;
          if (declaration->params.back() != p) { 
            pStr += ", "; 
          } else {
            pStr += "}";
          }
        }
        printIndent(); printIndent();
        log(pStr);
        printIndent(); printIndent();
        log("body: {");
        for (auto stmt : declaration->body) {
          printAST(stmt, indent + 1);
        }
        printIndent();
        log("}");
        break;
      }
      case NodeType::IDENTIFIER_LITERAL: {
        auto ident = dynamic_cast<const Identifier*>(node);
        printIndent();
        log("Identifier: {", ident->symbol, "}");
        break;
      }
      case NodeType::NUMERIC_LITERAL: {
        auto num = dynamic_cast<const NumericLiteral*>(node);
        printIndent();
        log("NumericLiteral: {", num->value, "}");
        break;
      }
      case NodeType::STRING_LITERAL: {
        auto str = dynamic_cast<const StringLiteral*>(node);
        printIndent();
        log("StringLiteral: {", str->value, "}");
        break;
      }
      case NodeType::BINARY_EXPRESSION: {
        auto bin = dynamic_cast<const BinaryExpression*>(node);
        printIndent();
        log("BinaryExpression: ", bin->op, " {");
        printAST(bin->left, indent + 1);
        printAST(bin->right, indent + 1);
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
};