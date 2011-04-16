#ifndef COMMONTOKEN_H
#define COMMONTOKEN_H

#include <common/string.h>
#include <map>

#include <stdint.h>

namespace Common {

template <typename TokenType>
class Token {
public:
  
  Token()
  {
  }
  
  Token(const TokenType &type,int line, int column, const String &lexema):
  _column(column),
  _lexema(lexema),
  _line(line),
  _type(type)
  {
  }
  /* o token é válido? */
  bool isValid() 
  {
  }
  
  TokenType getType() const
  {
    return _type;
  }
  
  String getLexema() const
  {
    return _lexema;
  }
  
  int getColumn() const
  {
    return _column;
  }
  
  int getLine() const
  {
    return _line;
  }
  
  void setType(const TokenType &type)
  {
    _type = type;
  }
  
private:
  /* A classe de um token */
  TokenType _type;
  
  /* O lexema, ou string achada */
  String _lexema;
  
  /* em que linha o token foi encontrado */
  int _line;
  
  /* e em que coluna inicia */
  int _column;
};

}

#endif
