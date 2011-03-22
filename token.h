#pragma once

#include <string>

namespace Lexical {

class Token {
public:
  /* Quais as classes de token? */
  enum type {
    IDENTIFIER,
    RESERVED,
    SYMBOL,
    CONSTANT,
    STRING,
    COMMENT
  };

private:
  std::tring _name;
  std::string _lexema;
  std::string _pattern; 
};

}
