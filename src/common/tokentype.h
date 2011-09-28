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
  * TODO: Como tratar linguagens que são case-insensitive?
  * Adicionar um parâmetro no constructor que diz se é para checar
  * sensitividade à caixa. Na hora do find, 
  * buscar a string transformada em "uppercase"
*/
template<typename TokenType>
class TokenHash
{
  /* um tipo de token inválido */
  TokenType _none;
  
  /* um mapa de palesvras reservadas */
  typedef std::map<const String,TokenType> ReservedMap;
  ReservedMap _reservedMap;
  
public:
  
  /* construtor que recebe um token nulo e um mapa lexema => tokentype */
  TokenHash(const TokenType &none, const ReservedMap &map):
  _none(none),
  _reservedMap(map)
  {
  }
  
  /* precisa de um Token inválido, que serve como referência */
  TokenHash(const TokenType &none):
  _none(none)
  {
  }
 
  /* adiciona uma nova palavra reservada*/
  virtual void add(const String &pattern,const TokenType &token)
  {
    _reservedMap.insert(std::pair<String,TokenType>(pattern,token));
  }
  
  /* recebe um lexema e diz qual token é */
  virtual TokenType find(const String &pattern) const {
    typename ReservedMap::const_iterator it(_reservedMap.find(pattern));
    return (it != _reservedMap.end()) ? it->second : _none;
  }
  
  /* Retorna o token que não indica coisa válida alguma */
  virtual TokenType getNone() const
  {
    return _none;
  }
};

}

#endif // COMMON_TOKENTYPE_H
