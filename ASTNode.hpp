#pragma once

#include <assert.h>
#include <vector>
#include <string>
#include <iostream>

#include "lexer.hpp"
#include "SymbolTable.hpp"

using emplex::Lexer;
using emplex::Token;

class ASTNode {
private:
  Token token;
  std::vector<ASTNode> children{};

public:
  ASTNode() : token(Token{'{', "Statement Block", 0}) { }
  ASTNode(Token token) : token(token) { }
  ASTNode(Token token, ASTNode child) : token(token) {
    AddChild(child);
  }
  ASTNode(Token token, ASTNode child1, ASTNode child2) : token(token) {
    AddChild(child1);
    AddChild(child2);
  }

  void AddChild(ASTNode child) {
    children.push_back(child);
  }

  void DoAssign(const std::string& value, SymbolTable & symbols) {
    if (token != Lexer::ID_IDENTIFIER) {
      std::cerr << "ERROR (line " << token.line_id << "): Cannot assign a value to '"
                << token.lexeme << "'." << std::endl;
      exit(1);
    }

    symbols.SetSymbol(token.lexeme, value);
  }

  std::string Run_Assign(SymbolTable & symbols) {
    assert(children.size() == 2);
    std::string rhs_value = children[1].Run(symbols);
    children[0].DoAssign(rhs_value, symbols);
    return rhs_value;
  }

  std::string Run_Block(SymbolTable & symbols) {
    for (ASTNode & child : children) child.Run(symbols);
    return "";
  }

  std::string Run_String([[maybe_unused]] SymbolTable & symbols) {
    std::string str = token.lexeme;
    // Remove quotes
    if (str.length() >= 2 && ((str[0] == '"' && str.back() == '"') || (str[0] == '\'' && str.back() == '\''))) {
      return str.substr(1, str.length() - 2);
    }
    return str;
  }

  std::string Run_Op(SymbolTable & symbols) {
    assert(children.size() == 2);
    std::string val1 = children[0].Run(symbols);
    std::string val2 = children[1].Run(symbols);

    if (token.lexeme == "+") {
      return val1 + val2;
    }
    if (token.lexeme == "-") {
      size_t pos = val1.find(val2);
      if (pos != std::string::npos) {
        return val1.substr(0, pos) + val1.substr(pos + val2.length());
      }
      return val1;
    }
    if (token.lexeme == "/") {
      size_t pos = val1.find(val2);
      if (pos != std::string::npos) {
        return val1.substr(0, pos);
      }
      return val1;
    }
    if (token.lexeme == "%") {
      size_t pos = val1.find(val2);
      if (pos != std::string::npos) {
        return val1.substr(pos + val2.length());
      }
      return "";
    }

    std::cerr << "Unknown binary operator '" << token.lexeme << "'.\n";
    exit(1);
    return "";
  }

  std::string Run_Print(SymbolTable & symbols) {
    for (ASTNode & child : children) {
      std::cout << child.Run(symbols) << std::endl;
    }
    return "";
  }

  std::string Run_Variable(SymbolTable & symbols) {
    return symbols.GetSymbolValue(token.lexeme);
  }

  std::string Run(SymbolTable & symbols);
};