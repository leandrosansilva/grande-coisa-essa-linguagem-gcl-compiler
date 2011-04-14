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

namespace Lexical {

using namespace Common;
  
template<typename T>
class Transition
{
public:
  T _from;
  T _to;
  String _pattern;
  
  Transition(T from, T to, const String &s):
  _from(from),
  _to(to),
  _pattern(s)
  {
  }
};
  
/* FIXME: implementação com array é porca.
 * TODO: reimplementar de forma mais eficiente
 * 
*/
template<typename T, typename Ttoken>
class TransitionTable
{
  typedef std::vector< Transition<T> > transitionVector;
  typedef std::map<T,Ttoken> stateTokenMap;
  
  T _startState;
  T _invalidState;
  T _finalState;
  T _currentState;
  T _previousState;
  transitionVector _table;
  stateTokenMap _matchedStates;
  String _matchedString;
  
public:
  /* Please tell me the initial, invalid and final states! */
  TransitionTable(T start, T invalid, T final):
  _startState(start),
  _invalidState(invalid),
  _finalState(final),
  _currentState(start)
  {
  }
  
  /* Adiciona um estado que casa um padrão */
  virtual void addMatched(T state, Ttoken tokenType)
  {
    _matchedStates[state] = tokenType;
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
  
  virtual void addTransition(T from, const String &s, T to)
  {
    _table.push_back(Transition<T>(from,to,s));
  }
  
  virtual void addFinalTransition(T from, const String &s,Ttoken token)
  {
    addTransition(from,s,_finalState);
    addMatched(from,token);
  }
  
  virtual T doTransition(char symbol)
  {
    _previousState = _currentState;
    
    /* Acha o elemento */
    typename transitionVector::iterator i(_table.begin());
    for (; i != _table.end(); i++) {
      if (i->_from == _currentState && i->_pattern.hasChar(symbol))
        break;
    }
    
    _currentState = i != _table.end() ? i->_to : _invalidState;
    
    if (isInAValidState() && !isInAMatchedState())
      _matchedString += symbol;
    
    //std::cout << "read '" << symbol << "' and changed to state " << _currentState << std::endl;
      
    return _currentState;
  }
  
  /* reseta estado ao inicial e limpa o buffer do lexema  */
  virtual void reset()
  {
    _currentState = _startState;
    _matchedString = "";
  }
  
  virtual T getCurrentState() const
  {
    return _currentState;
  }
  
  virtual T getPreviousState() const 
  {
    return _previousState;
  }
  
  virtual String getMatchedString() const 
  {
    return _matchedString;
  }
  
  virtual Ttoken getMatchedToken() const 
  {
    /* Casei. meu estado final não me diz nada 
     * Mas _previousState aponta para o último estado que eu estava.
     * Busco _previousState no mapa e obtenho um iterator.
     * Busco o segundo termo deste iterator
     */
    return _matchedStates.at(_previousState);
  }
};
}
#endif
