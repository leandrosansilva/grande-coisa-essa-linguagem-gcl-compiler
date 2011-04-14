#ifndef LEXIXALTRANSITIONTABLE
#define LEXIXALTRANSITIONTABLE

#include "string.h"
#include <vector>
#include <iostream>
#include <algorithm>

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
  typedef std::vector<T> stateVector;
  
  T _start;
  T _invalid;
  T _final;
  T _currentState;
  T _previousState;
  transitionVector _table;
  stateVector _matchedStates;
  String _matchedString;
  
public:
  /* Please tell me the initial, invalid and final states! */
  TransitionTable(T start, T invalid, T final):
  _start(start),
  _invalid(invalid),
  _final(final),
  _currentState(start)
  {
  }
  
  /* Adiciona um estado de casa um padrão */
  void addMatched(T state, Ttoken tokenType)
  {
    _matchedStates.push_back(state);
  }
  
  /* retorna true se o estado atual é um dos finais positivamente,
   * onde não tem mais pra onde ir */
  bool isInAMatchedState()
  {
    return std::find(_matchedStates.begin(),_matchedStates.end(),_currentState) != _matchedStates.end();
  }
  
  bool isInAValidState()
  {
    return _currentState != _invalid;
  }
  
  void addTransition(T from, const String &s, T to)
  {
    _table.push_back(Transition<T>(from,to,s));
  }
  
  void addFinalTransition(T from, const String &s,Ttoken token)
  {
    addTransition(from,s,_final);
    addMatched(_final,token);
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
    
    _currentState = i != _table.end() ? i->_to : _invalid;
    
    if (_currentState != _invalid && _currentState != _final)
      _matchedString += symbol;
    
    //std::cout << "read '" << symbol << "' and changed to state " << _currentState << std::endl;
      
    return _currentState;
  }
  
  /* reseta estado ao inicial e limpa o buffer do lexema  */
  void reset()
  {
    _currentState = _start;
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
  
};
}
#endif