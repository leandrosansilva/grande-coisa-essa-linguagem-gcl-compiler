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


#ifndef LEXICAL_ANALYSER_H
#define LEXICAL_ANALYSER_H

#include "filereader.h"

#include <common/token.h>

#include <common/tokentype.h>

#include "transitiontable.h"

#include <list>

#include <algorithm>

namespace Lexical {
  
using namespace Common;
  
template<typename T, typename Ttoken>
class Analyser
{
  FileReader &_file;
  
  /* autômato */
  TransitionTable<T,Ttoken> &_table;
  
  /* hash de palavras reservadas */
  TokenHash<Ttoken> &_reserved;
  
  /* Lista de tokens que devem ser ignorados */
  typedef std::list<Ttoken> IgnoreList;
  IgnoreList _ignore;
  
  /* Método privado para pegar o próximo token */
  Token<Ttoken> _privGetToken() 
  {
    int column(_file.getColumnNumber());
    int line(_file.getLineNumber()); 
    while (_file.canRead()) {
      char c(_file.getChar());
      _table.doTransition(c);
      if (_table.isInAValidState()) {
        if (_table.isInAMatchedState()) {
          /* Casou. Retorno token */
          Token<Ttoken> t(
            _table.getMatchedToken(),
            line,
            column,
            _table.getMatchedString()
          );
          /* Reseta o automato pro estado inicial*/
          _table.reset();
          /* Volta uma posição na entrada */
          _file.backOnePosition();
          return t;
        } 
      } else {
        return Token<Ttoken>(
          _table.getMatchedToken(),
          line,
          column,
          _table.getMatchedString()
        );
      }
    }
  }

public:

  Analyser(FileReader &file, TransitionTable<T,Ttoken> &table, TokenHash<Ttoken> &reserved):
  _file(file),
  _table(table),
  _reserved(reserved)
  {
  }
  
  void ignoreToken(Ttoken &token)
  {
    _ignore.push_back(token);
  }
  
  /* me retorna se posso continuar a pegar token ou se já terminou */
  bool canReadToken() const 
  {
    /* Só retorna se é capaz de ler do arquivo */
    return _file.canRead();
  }
 
  /* Obtem o próximo token, 
   * mas ignora aqueles que estão na lista de "ignorados"
   * e já olha na tabela de palavras reservadas
  */
  Token<Ttoken> getToken()
  {
    Token<Ttoken> t;
    do {
      t = _privGetToken();
    } while (std::find(_ignore.begin(), _ignore.end(),t.getType()) != _ignore.end());
    
    return t;
  }
};

}

#endif // LEXICAL_ANALYSER_H
