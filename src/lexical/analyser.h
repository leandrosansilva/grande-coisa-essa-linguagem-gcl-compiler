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

#include "input.h"
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
  /* De onde vêm os caracteres? */
  Input &_input;
  
  /* autômato */
  TransitionTable<StateType,TokenType> &_table;
  
  /* hash de palavras reservadas */
  TokenHash<TokenType> &_reserved;
  
  /* controla se ainda pode haver leitura de token */
  bool _canRead;
  
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
  
  /* Lista de tipos de tokens a serem comparados na tabela de reservados */
  TokenTypeList _compare;
  
  /* Lista de tipos de tokens a serem ignorados */
  TokenTypeList _ignore;
  
  /* Método privado para pegar o próximo token 
   * FIXME: OMG! OMG! Que método graaande! :-(
   * TODO: refatorar!
   */
  Token<TokenType> _privGetToken()
  {
    /* Já gravo a linha e coluna onde inicio o a leitura do token */
    int column(_input.getColumnNumber());
    int line(_input.getLineNumber());
    
    while (_input.canRead()) {
      char c(_input.getChar());
      
      _table.doTransition(c);
      
      /* Verifico se existe uma marca no autômato, 
       * de um estado anterior em conflito
       * 
       * Mas o estado conflitante desta marca deve ser 
       * igual ao estado atual
       */
      if (_table.hasMark(_table.getCurrentState())) {
        
        /*std::cout << "O Autômato tem uma marca em " 
                  << "'" << _table.getMarkPos() << "' => "
                  << "'" << _table.getMarkState() << "'"
                  << std::endl;*/
                  
        /* Devo voltar quantos caracteres? (tanto no lexema quanto na entrada) */
        int backSize(_input.getPos() - _table.getMarkPos());
        
        /* String casada no autômato, com os caracteres errados no final */
        String matched(_table.getMatchedString());
        
        /* Pego a string sem o final adicional gerado pelo conflito */
        String lexema(matched.substr(0,matched.size() - backSize));
        
        Token<TokenType> t(_table.getTokenTypeFinishedIn(_table.getMarkState()),line,column,lexema);
        
        _input.back(backSize);
        _table.reset();
        
        return t;
      }
      
      /* Se o estado atual poderá causar um conflito futuro,
       * insiro uma "marca" no autômato
      */
      if (_table.hasConflict(_table.getCurrentState())) {
        _table.setMark(_table.getCurrentState(),_input.getPos());
      }
      
      /* se o automato está num estado inválido, é pau */
      if (!_table.isInAValidState()) {
        /* se não posso mais ler do arquivo, informa isso */
        if (!_input.canRead()) {
          _canRead = false;
        }
        
        /* Se o estado anterior for diferente de inicial, ou seja,
         * se li ao menos um caractere, volto uma posição
        */
        if ((_table.getPreviousState() != _table.getInitialState())) {
          _input.back();
        }
        break;
      }
      
      /* Se casou, retorno o token correto e saio */
      if (_table.isInAMatchedState()) {
        /* Casou. Retorno token */
        Token<TokenType> t(_table.getMatchedToken(),line,column,_table.getMatchedString());
        
        /* Reseta o automato pro estado inicial */
        _table.reset();
        
        /* se não posso mais ler do arquivo, informa isso */
        if (!_input.canRead()) {
          _canRead = false;
        }
        
        //std::cout << "Size: " << _input.getSize() << ", pos: " << _input.getPos() << std::endl;
        
        /* Volto uma posição na entrada*/
        _input.back();

        return t;
      }
    } // _file.canRead()
    

    /* Se chegou até aqui, retorna um token inválido */
    Token<TokenType> t(_reserved.getNone(),line,column,_table.getMatchedString());
    
    _table.reset();
    
    return t;
  }

public:

  Analyser(Input &file, TransitionTable<StateType,TokenType> &table, TokenHash<TokenType> &reserved):
  _input(file),
  _table(table),
  _reserved(reserved),
  _canRead(true)
  {
  }
  
  void addTokenToCompareWithReserved(const TokenType &t)
  {
    _compare.add(t);
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
    return _canRead;
  }
 
  /* Obtem o próximo token,
   * mas ignora aqueles que estão na lista de "ignorados"
   * e já olha na tabela de palavras reservadas
  */
  Token<TokenType> getToken()
  {
    Token<TokenType> token;
    
    /* Lê token até achar um que não possa ser descartado */
    do {
      token = _privGetToken();
    } while (_ignore.find(token.getType()));
    
    /* se foi pego como identificador, vejo se é uma palavra reservada */
    if (_compare.find(token.getType())) {
        
      /* o token da string que casou. Deve ser diferente do tipo inválido */
      TokenType foundType(_reserved.find(token.getLexema()));
      if (foundType != _reserved.getNone()) {
        /* Se for uma palavra reservada, defino o novo tipo do token encontrado */
        token.setType(foundType);
      }
    }
    
    /* Retorna o token, seja ele válido ou inválido */
    return token;
  }
};

}

#endif // LEXICAL_ANALYSER_H
