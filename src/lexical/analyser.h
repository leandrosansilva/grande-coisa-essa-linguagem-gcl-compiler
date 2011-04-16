/*
    Implementação de um analisador léxico genérico
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
  
template<typename StateType, typename TokenType>
class Analyser
{
  FileReader &_file;
  
  /* autômato */
  TransitionTable<StateType,TokenType> &_table;
  
  /* hash de palavras reservadas */
  TokenHash<TokenType> &_reserved;
  
  /* Lista de tokens que devem ser ignorados */
  class TokenTypeList
  {
    std::list<TokenType> _list;
    
  public:
    bool find(const TokenType &type)
    {
      return std::find(_list.begin(),_list.end(),type) != _list.end();
    }
    
    void add(const TokenType &type)
    {
      _list.push_back(type);
    }
  };
  
  TokenTypeList _listTypeTokenToCompare;
  TokenTypeList _ignore;
  
  /* Método privado para pegar o próximo token */
  Token<TokenType> _privGetToken() 
  {
    /* Já gravo a linha e coluna onde inicio o a leitura do token */
    int column(_file.getColumnNumber());
    int line(_file.getLineNumber());
    
    while (_file.canRead()) {
      char c(_file.getChar());
      _table.doTransition(c);
      
      if (_table.isInAValidState()) {
        if (_table.isInAMatchedState()) {
          /* Casou. Retorno token */
          Token<TokenType> t(
            _table.getMatchedToken(),
            line,
            column,
            _table.getMatchedString()
          );
          /* Reseta o automato pro estado inicial*/
          _table.reset();
          
          /* Volto uma posição na entrada*/
          _file.backOnePosition();

          return t;
        }
      } else { // isInAValidState()

        /* Se o estado anterior for diferente de inicial, ou seja,
         * se li ao menos um caractere, volto uma posição
        */
        if (_table.getPreviousState() != _table.getInitialState()) {
          _file.backOnePosition();
        }
        break;
      }
    } // _file.canRead()
    
    /* Se chegou até aqui, retorna um token inválido */
    Token<TokenType> t(
      _reserved.getNone(),
      line,
      column,
      _table.getMatchedString()
    );
    
    _table.reset();
    
    return t;
  }

public:

  Analyser(FileReader &file, TransitionTable<StateType,TokenType> &table, TokenHash<TokenType> &reserved):
  _file(file),
  _table(table),
  _reserved(reserved)
  {
  }
  
  void addTokenToCompareWithReserved(const TokenType &t)
  {
    _listTypeTokenToCompare.add(t);
  }
 
  /* Deve ignorar o tipo passado */
  void ignoreToken(const TokenType &token)
  {
    _ignore.add(token);
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
  Token<TokenType> getToken()
  {
    Token<TokenType> t;
    
    /* Lê token até achar um que não possa ser descartado */
    do {
      t = _privGetToken();
    } while (_ignore.find(t.getType()));
    
    
    /* se foi pego como identificador, vejo se é uma palavra reservada */
    if (_listTypeTokenToCompare.find(t.getType())) {
        
      /* o token da string que casou. Deve ser diferente do tipo inválido */
      TokenType foundType(_reserved.find(t.getLexema()));
      if (foundType != _reserved.getNone()) {
        /* Se for uma palavra reservada, defino o novo tipo do token encontrado */
        t.setType(foundType);
      }
    }
    
    /* Retorna o token, seja ele válido ou inválido */
    return t;
  }
};

}

#endif // LEXICAL_ANALYSER_H
