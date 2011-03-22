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
  /* O que um token tem? */
  std::tring _name;
  std::string _lexema;
  std::string _pattern; 
};

}
