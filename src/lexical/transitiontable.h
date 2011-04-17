/*
    Implements a transition table!
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

#ifndef LEXIXALTRANSITIONTABLE
#define LEXIXALTRANSITIONTABLE

#include <common/string.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>

#include <exception>

namespace Lexical {

using namespace Common;
  
template<typename StateType>
class Transition
{
public:
  StateType _from;
  StateType _to;
  String _pattern;
  
  Transition(const StateType &from, const StateType &to, const String &s):
  _from(from),
  _to(to),
  _pattern(s)
  {
  }
};
  
/* 
 * TODO: reimplementar de forma mais eficiente
 * 
*/
template<typename StateType, typename TokenType>
class TransitionTable
{
  typedef std::vector< Transition<StateType> > TransitionVector;
  typedef std::map<StateType,TokenType> StateTokenMap;
  
  StateType _startState;
  StateType _invalidState;
  StateType _finalState;
  StateType _currentState;
  
  /* O estado antes de fazer a transição.
   * É por ele que sei por qual estado saí do autômato
  */
  StateType _previousState;
  
  TransitionVector _table;
  StateTokenMap _matchedTokens;
  String _matchedString;
  
  /* Adiciona um estado que casa um padrão */
  virtual void _addMatched(const StateType &state, const TokenType &tokenType)
  {
    _matchedTokens[state] = tokenType;
  }
  
public:
  /* Please tell me the initial, invalid and final states! */
  TransitionTable(const StateType &start, const StateType &invalidState, const StateType &final):
  _startState(start),
  _invalidState(invalidState),
  _finalState(final),
  _currentState(start)
  {
  }
  

  
  /* retorna true se o estado atual é um dos finais positivamente,
   * onde não tem mais pra onde ir */
  virtual bool isInAMatchedState() const
  {
    return _currentState == _finalState;
  }
  
  virtual bool isInAValidState() const
  {
    return _currentState != _invalidState;
  }
  
  virtual void addTransition(const StateType &from, const String &s, const StateType &to)
  {
    _table.push_back(Transition<StateType>(from,to,s));
  }
  
  virtual void addFinalTransition(const StateType &from, const String &s,const TokenType &token)
  {
    addTransition(from,s,_finalState);
    _addMatched(from,token);
  }
  
  /* Efetua uma transição. 
   * Retorna o estado inválido caso não 
   * tenha conseguido fazer esta transição
   */
  virtual StateType doTransition(const char &symbol)
  {
    _previousState = _currentState;
    
    /* Acha o elemento */
    typename TransitionVector::iterator i(_table.begin());
    for (; i != _table.end(); i++) {
      if (i->_from == _currentState && String(i->_pattern + "\0").hasChar(symbol))
        break;
    }
    
    /* retorna a primeira transição que encontrou. Ou inválido, caso contrário */
    _currentState = i != _table.end() ? i->_to : _invalidState;
   
    /* só concateno a string achada quando não for um estado depois do final 
     * ou caso tenha entrado num estado inválido
     */
    if (!isInAMatchedState())
      _matchedString += symbol;
    
    /*std::cout << "was in state '" << _previousState << "' ,read " 
              << (int)symbol << " -> '" << symbol 
              <<  "' and changed to state " << _currentState << std::endl;*/
      
    return _currentState;
  }
  
  /* reseta estado ao inicial e limpa o buffer do lexema  */
  virtual void reset()
  {
    _currentState = _startState;
    _matchedString = "";
  }
  
  virtual StateType getCurrentState() const
  {
    return _currentState;
  }
  
  virtual StateType getPreviousState() const 
  {
    return _previousState;
  }
  
  virtual String getMatchedString() const 
  {
    return _matchedString;
  }
  
  virtual StateType getInitialState() const 
  {
    return _startState;
  }
  
  virtual TokenType getMatchedToken() const 
  {
    /* _previousState aponta para o último estado que eu estava.
     * Logo é ele que me diz por qual estado saí do autômato
    */
    return _matchedTokens.at(_previousState);
  }
};
}
#endif
