#ifndef COMMONTOKEN_H
#define COMMONTOKEN_H

#include <common/string.h>
#include <map>

#include <stdint.h>

namespace Common {

template <typename Ttoken>
class Token {
public:
  
  Token()
  {
  }
  
  Token(Ttoken type,int line, int column, String lexema):
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
  
  Ttoken getType() const
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
  
  void setType(Ttoken type)
  {
    _type = type;
  }
  
private:
  /* A classe de um token */
  Ttoken _type;
  
  /* O lexema, ou string achada */
  String _lexema;
  
  /* em que linha o token foi encontrado */
  int _line;
  
  /* e em que coluna inicia */
  int _column;
};

}

#endif
