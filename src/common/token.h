#ifndef COMMONTOKEN_H
#define COMMONTOKEN_H

#include <common/string.h>
#include <map>

#include <stdint.h>

namespace Common {

/* o padding sobre o lexema de um token */
struct TokenPadding
{
  TokenPadding(const int &left, const int &right):
  _left(left),
  _right(right)
  {}
  
  int _left;
  int _right;
};

/* mapa de padding de cada tipo de token */
template<typename TokenType>
class TokenPaddingMap: public std::map<TokenType,TokenPadding>
{
public:
  void add(const TokenType &type, const TokenPadding &padding)
  {
    insert(std::pair<TokenType,TokenPadding>(type,padding));
  }
};


template <typename TokenType>
class Token {
public:
  
  Token()
  {
  }
  
  Token(const TokenType &type,int line, const String &lexema):
  _lexema(lexema),
  _line(line),
  _type(type)
  {
  }
  
  const TokenType getType() const
  {
    return _type;
  }
  
  String getLexema() const
  {
    return _lexema;
  }
  
  const int getLine() const
  {
    return _line;
  }
  
  void setType(const TokenType &type)
  {
    _type = type;
  }
  
  void setPadding(const TokenPadding &padding)
  {
    _lexema = _lexema.substr(padding._left,_lexema.size() - padding._right - padding._left );
  }
  
private:
  /* A classe de um token */
  TokenType _type;
  
  /* O lexema, ou string achada */
  String _lexema;
  
  /* em que linha o token foi encontrado */
  int _line;
};

}

#endif
