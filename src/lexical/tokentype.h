/*
    <one line to give the program's name and a brief idea of what it does.>
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

#ifndef LEXICAL_TOKENTYPE_H
#define LEXICAL_TOKENTYPE_H

#include <map>
#include "string.h"

namespace Lexical {
/*
  * Palavras reservadas da GCL:
  * 
  * module private end const Boolean integer begin typedef array range
  * proc val ref return write read if fi do od true false forall llarof skip
  * 
  * Símbolos:
  * 
  * .   =   :=  --  ,   ;   (   ) []  [   ]   ->  #
  * < > <=  >=  &   |   ~   +   -   *   /   \  ..
  * 
  * Comentários:
  * -- comentário
  *
  * Um programa é uma sequência de módulos. Definição de módulo:
  * <module> ->
  * module identifier
  * <definitionPart>
  * private
  * <block> .
*/
typedef enum {
  /* Nenhum Token, útil como resultado de busca? */
  None = 0,
  
  /* Palavras reservadas */
  Module,
  Private,
  End,
  Const,
  Boolean,
  Integer,
  Begin,
  Typedef,
  Array,
  Range,
  Proc,
  Val,
  Ref,
  Return,
  Write,
  Read,
  If,
  Fi,
  Do,
  Od,
  True,
  False,
  Forall,
  Llarof,
  Skip,
  
  /* Símbolos */
  Dot, // .
  Equal, // =
  Assign, // :=
  Comment, // --
  Comma, // ,
  Semicolon, // ;
  LParentesis, // (
  RParentesis, // )
  LBracket, // [
  RBracket, // ]
  Else, // [] // else
  Then, // -> // then
  Sharp, // #
  Lesser, // <
  Greater, // >
  LessEqual, // <=
  GreaterEqual, // >=
  And, // &
  Or, // |
  Til, // ~
  Plus, // +
  Minus, // -
  Asterisk, // *
  Slash, // /
  BackSlash, // \\ 
  TwoDot, // ..
 
  /* Tipos constantes, números, etc. */
  ClassInteger,
  ClassReal,
  ClassString,
  ClassId,
  ClassComment,
  ClassSpaces
  
} TokenType;

/* 
  * Tabela hash com todos as palavras reservadas
  * 
  * TODO: como GCL não é case-sensitive,
  * devo fazer todas as operações com lowercase
*/
template<typename T>
class TokenHash
{
  T _none;
  typedef std::map<const String,T> rMap;
  rMap _reservedMap;
  
public:
  TokenHash(T none):
  _none(none)
  {
  }
 
  virtual void add(const String &pattern,T token)
  {
    _reservedMap[pattern] = token;
  }
  
  virtual T findReservedWord(const String &pattern) const {
    typename rMap::const_iterator it(_reservedMap.find(pattern));
    return (it != _reservedMap.end()) ? it->second : _none;
  }
};

}

#endif // LEXICAL_TOKENTYPE_H
