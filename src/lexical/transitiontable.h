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
  transitionVector _table;
  stateVector _matchedStates;
  
public:
  /* Please tell me the initial, invalid and final states! */
  TransitionTable(T start, T invalid, T final):
  _start(start),
  _invalid(invalid),
  _final(final)
  {
  }
  
  /* Adiciona um estado de casa um padrão */
  void addMatched(T state, Ttoken tokenType)
  {
    _matchedStates.push_back(state);
  }
  
  /* retorna true se o estado atual é um dos finais positivamente,
   * onde não tem mais pra onde ir */
  bool isMatchedState(T state)
  {
    return std::find(_matchedStates.begin(),_matchedStates.end(),state) != _matchedStates.end();
  }
  
  bool hasStateAsValid(T state)
  {
    return state != _invalid;
  }
  
  void addTransition(T from, const String &s, T to)
  {
    _table.push_back(Transition<T>(from,to,s));
  }
  
  void addFinalTransition(T from, const String &s,Ttoken final)
  {
    _matchedStates.push_back(final);
  }
  
  T doTransition(T from, char symbol)
  {
    /* Acha o elemento */
    typename transitionVector::iterator i(_table.begin());
    for (; i != _table.end(); i++) {
      if (i->_from == from && i->_pattern.hasChar(symbol))
        break;
    }
    
    return i != _table.end() ? i->_to : _invalid;
  }
  
  /* reseta estado ao inicial e limpa o buffer do lexema  */
  void reset(T &s, String &match, int &i)
  {
    s = _start;
    match = "";
    i--;
  }
};
}
#endif