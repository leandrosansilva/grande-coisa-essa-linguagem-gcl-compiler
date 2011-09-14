#ifndef SYNTATIC_AUTOMATA_H
#define SYNTATIC_AUTOMATA_H

#include "grammar.h"

#include <src/common/token.h>

#include <stack>

using namespace std;

using namespace Common;

namespace Syntatical {

template<typename NonTerminalT, typename TerminalT>
struct Automata
{
  /* FIXME: isto está feio. As duas classes ficam muito desconexas assim */
  typedef typename Grammar<NonTerminalT,TerminalT>::Symbol Symbol;

  typedef typename Grammar<NonTerminalT,TerminalT>::LR1Table LR1Table;

  typedef typename Grammar<NonTerminalT,TerminalT>::LR1Key LR1Key;
  
  LR1Table _table;

  stack<int> _stateStack;
  
  stack<Symbol> _symbolStack;

  Grammar<NonTerminalT,TerminalT> _grammar;

  /* o autômato recebe uma uma gramática */
  Automata(Grammar<NonTerminalT,TerminalT> &grammar):
  /* e já gera a tabela de ações/goto -> processo demorado */
  _table(grammar.createTable()),

  /* inicialmente possuo o estado 0 na pilha */
  _stateStack({0}),
  _grammar(grammar)
  {
  }

  void readToken(const Token<TerminalT> &token)
  {
    cout << _grammar._symbolToString(token.getType()) 
         << ": \"" <<  token.getLexema() << "\"" << endl;

    // Ação a ser executada: Reduce, error, goto, accept
    typename LR1Table::iterator action(_table.find(LR1Key(_stateStack.top(),{token.getType()})));

    if (action == _table.end()) {
      cout << "Erro no parser!" << endl;
      return;
    }

    // aceitação, casou tudo
    if (action->second._action == ACCEPT) {
      cout << "Aceitou!" << endl;
      return;
    }

    // ação de shift
    if (action->second._action == SHIFT) {
      cout << "Empilho" << endl;
      _stateStack.push(action->second._value);
      _symbolStack.push(token.getType());
      return;
    }

    // ação de reduce
    if (action->second._action == REDUCE) {
      int popSize(_grammar._v[action->second._value]._production.size());

      // o lado esquerdo da produção do reduce 
      Symbol leftSide(_grammar._v[action->second._value]._leftSide);

      cout << "Preciso remover da pilha " 
           << popSize 
           << ", minha pilha de símbolos possui " << _symbolStack.size()
           << " e a pilha de estados possui " << _stateStack.size() << endl;

      /* desempilho x elementos  */
      for (int i(0); i<popSize; i++) {
        _symbolStack.pop();
        _stateStack.pop();
      }

      /* insere na pilha o lado esquerdo (A) da produção */
      _symbolStack.push(leftSide);

      /* insere na pilha o GOTO[topo,A]. TODO: pode dar erro?  */ 
      _stateStack.push(_table[LR1Key(_stateStack.top(),leftSide)]._value);
    }
  }
};
}
#endif
