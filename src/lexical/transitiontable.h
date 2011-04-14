#ifndef LEXIXALTRANSITIONTABLE
#define LEXIXALTRANSITIONTABLE

#include "string.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>

namespace Lexical {
  
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
  void addMatched(T state, Ttoken tokenType)
  {
    _matchedStates[state] = tokenType;
  }
  
  /* retorna true se o estado atual é um dos finais positivamente,
   * onde não tem mais pra onde ir */
  bool isInAMatchedState()
  {
    return _currentState == _finalState;
  }
  
  bool isInAValidState()
  {
    return _currentState != _invalidState;
  }
  
  void addTransition(T from, const String &s, T to)
  {
    _table.push_back(Transition<T>(from,to,s));
  }
  
  void addFinalTransition(T from, const String &s,Ttoken token)
  {
    addTransition(from,s,_finalState);
    addMatched(from,token);
  }
  
  T doTransition(char symbol)
  {
    _previousState = _currentState;
    
    /* Acha o elemento */
    typename transitionVector::iterator i(_table.begin());
    for (; i != _table.end(); i++) {
      if (i->_from == _currentState && i->_pattern.hasChar(symbol))
        break;
    }
    
    _currentState = i != _table.end() ? i->_to : _invalidState;
    
    if (_currentState != _invalidState && _currentState != _finalState)
      _matchedString += symbol;
    
    //std::cout << "read '" << symbol << "' and changed to state " << _currentState << std::endl;
      
    return _currentState;
  }
  
  /* reseta estado ao inicial e limpa o buffer do lexema  */
  void reset()
  {
    _currentState = _startState;
    _matchedString = "";
  }
  
  T getCurrentState() const 
  {
    return _currentState;
  }
  
  T getPreviousState() const 
  {
    return _previousState;
  }
  
  String getMatchedString() const 
  {
    return _matchedString;
  }
  
  Ttoken getMatchedToken() const 
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