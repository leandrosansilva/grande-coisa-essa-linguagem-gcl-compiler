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
  DoubleBracket, // []
  Pointer, // ->
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
  * TODO: Tabela hash com todos as palavras reservadas
  * ela terá a seguinte "cara":
  * class TokenMap: public std::map<String,TokenType>
*/
/*template<typename T>
class TokenHash: public std::map<const String,T>
{
  T _none;
  
public:
  TokenHash(T none):
  _none(none)
  {
  }
  
  T findReservedWord(const String &pattern) const {
    typename const_iterator it(find(pattern));
    return (it != end()) ? it->second : _none;
  }
};*/

}

#endif // LEXICAL_TOKENTYPE_H
