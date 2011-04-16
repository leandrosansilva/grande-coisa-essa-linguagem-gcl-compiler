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
  
  /* qual é o token que representa identificador 
   * e poderá ser uma palavra reservada */
  Ttoken _idToken;
  
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
      } else { // isInAValidState()
        
        std::cout << "Deu pau!" << std::endl;
        // Retorna um token inválido
        return Token<Ttoken>(
          _table.getMatchedToken(),
          line,
          column,
          _table.getMatchedString()
        );
      }
    } // _file.canRead()
    /* Se cheguei aqui, não posso mais ler o arquivo, pq acabou */
    std::cout << "Acabou o arquivo" << std::endl;
    /* Se acabou o arquivo, o que devo retornar? */
    std::cout << "Deu pau!" << std::endl;
    // Retorna um token inválido
    return Token<Ttoken>(
      _table.getMatchedToken(),
      line,
      column,
      _table.getMatchedString()
    );
  }

public:

  Analyser(FileReader &file, TransitionTable<T,Ttoken> &table, TokenHash<Ttoken> &reserved, Ttoken idToken):
  _file(file),
  _table(table),
  _reserved(reserved),
  _idToken(idToken)
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
    
    /* Lê token até achar um que não possa ser descartado */
    do {
      t = _privGetToken();
    } while (std::find(_ignore.begin(), _ignore.end(),t.getType()) != _ignore.end());
    
    /* se foi pego como identificador, vejo se é uma palavra reservada */
    if (t.getType() == _idToken) {
      
      /* o token da string que casou. Deve ser diferente do tipo inválido */
      Ttoken foundType(_reserved.find(t.getLexema()));
      if (foundType != _reserved.getNone()) {
        /* Se for uma palavra reservada, defino o novo tipo do token encontrado */
        t.setType(foundType);
      }
    }
    
    return t;
  }
};

}

#endif // LEXICAL_ANALYSER_H
