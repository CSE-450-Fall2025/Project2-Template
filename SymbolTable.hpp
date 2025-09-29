#pragma once

#include <map>
#include <string>
#include <iostream>

#include "lexer.hpp"
#include "helpers.hpp"

using emplex::Token;

class SymbolTable {
private:
  struct VarInfo {
    std::string value;
    size_t def_line;
  };

  std::map<std::string, VarInfo> symbols;

public:
  void AddSymbol(Token token) {
    if (symbols.find(token.lexeme) != symbols.end()) {
      Error(token.line_id, "Redeclaration of variable '", token.lexeme, "' (originally defined on line ", symbols[token.lexeme].def_line, ").");
    }
    symbols[token.lexeme].value = "";
    symbols[token.lexeme].def_line = token.line_id;
  }

  std::string GetSymbolValue(const std::string& name) {
    if (symbols.find(name) == symbols.end()) {
      Error(0, "Unknown variable '", name, "'");
    }
    return symbols[name].value;
  }

  void SetSymbol(const std::string& name, const std::string& value) {
    if (symbols.find(name) == symbols.end()) {
      Error(0, "Unknown variable '", name, "'");
    }
    symbols[name].value = value;
  }
};