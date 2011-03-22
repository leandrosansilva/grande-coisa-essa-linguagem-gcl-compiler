#ifndef LEXICALTOKEN_H
#define LEXICALTOKEN_H

#include <string>
#include <map>

namespace Lexical {

/*
 * FIXME: estou confundindo a definição de um token com sua instância :-(
*/

/* mapa com uma expressão que gera um token
 * 
 * FIXME: isto não é um mapa de string para string
 * 
 * por exemplo:
 * ([_[:alpha:]])([_[:alnum:]])* => TokenIdentifier
*/
class TokenMap: public std::map<std::string,std::string> {
public:
  /* TODO: Definir aqui os métodos que possam ser úteis */
};

class Token {
public:
  /* Quais as classes de token? */
  enum type {
    IDENTIFIER, // identificador
    RESERVED, // palavra reservada
    SYMBOL, // símbolo
    CONSTANT, // constante
    STRING, // constante de texto?
    COMMENT // comentário: quem disse que comentários são inúteis? :-)
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