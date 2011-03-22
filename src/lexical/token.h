#ifndef LEXICALTOKEN_H
#define LEXICALTOKEN_H

#include <string>

namespace Lexical {

/*
 * FIXME: estou confundindo a definição de um token com sua instância :-(
*/

class Token {
public:
  /* Quais as classes de token? */
  enum type {
    IDENTIFIER, // identificador
    RESERVED, // palavra reservada
    SYMBOL, // símbolo
    CONSTANT, // constante
    STRING, // constante de texto?
    COMMENT // comentário: quem disse que comentários não são úteis? :-)
  };

private:
  /* O token de um identificador */
  type _type;
  
  /* O lexema, ou string achada */
  std::string _lexema;
  
  /* Qual padrão que casa o token */
  std::string _pattern; 
};

}

#endif