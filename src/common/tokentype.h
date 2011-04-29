/*
    Tabela Hash com palavras reservadas
    Copyright (C) 2011  Leandro Santiago <leandrosansilva@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef COMMON_TOKENTYPE_H
#define COMMON_TOKENTYPE_H

#include <map>
#include "string.h"

namespace Common {
/* 
  * Tabela hash com todos as palavras reservadas
  * Como tratar linguagens que não são case-sensitive? 
*/
template<typename TokenType>
class TokenHash
{
  TokenType _none;
  typedef std::map<const String,TokenType> ReservedMap;
  ReservedMap _reservedMap;
  
public:
  TokenHash(const TokenType &none):
  _none(none)
  {
  }
 
  virtual void add(const String &pattern,const TokenType &token)
  {
    _reservedMap[pattern] = token;
  }
  
  virtual TokenType find(const String &pattern) const {
    typename ReservedMap::const_iterator it(_reservedMap.find(pattern));
    return (it != _reservedMap.end()) ? it->second : _none;
  }
  
  virtual TokenType getNone() const
  {
    return _none;
  }
};

}

#endif // COMMON_TOKENTYPE_H
