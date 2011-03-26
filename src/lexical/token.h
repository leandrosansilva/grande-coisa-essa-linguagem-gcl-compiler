#ifndef LEXICALTOKEN_H
#define LEXICALTOKEN_H

#include <string>
#include <map>

#include <stdint.h>

namespace Lexical {

/*
 * FIXME: estou confundindo a definição de um token com sua instância :-(
*/

/* mapa com uma expressão que gera um token
 * 
 * FIXME: isto não deve um mapa de string para string!
 * 
 * por exemplo:
 * ([_[:alpha:]])([_[:alnum:]])* => TokenIdentifier
 * := => TokenAttribuition
*/

class TokenMap: public std::map<std::string,std::string> {
public:
  /* TODO: Definir aqui os métodos que possam ser úteis */
};

class Token {
public:
  /* Quais as classes de token? */
  enum tokenClass {
    IDENTIFIER, // identificador
    RESERVED, // palavra reservada
    SYMBOL, // símbolo
    CONSTANT, // constante
    STRING, // constante de texto?
    COMMENT // comentário: quem disse que comentários são inúteis? :-)
  };
  
  /* TODO: especificar qual token é, se é if, for, while, +,-,:=...
   * Cada um destes tipos de token tem uma expressão regular associada
   * Depois coloca isso no mapa
   * O problema é que cada tipo deve ficar numa especialização de token doferente.
   * Logo, mesmo o enum tokenClass deve sumir
  */
  
  /* o token é válido? */
  bool isValid();
  
private:
  /* A classe de um token */
  tokenClass _class;
  
  /* O lexema, ou string achada */
  std::string _lexema;
  
  /* em que linha o token foi encontrado */
  uint64_t _line;
  
  /* e em que coluna inicia */
  uint64_t _column;
};

}

#endif