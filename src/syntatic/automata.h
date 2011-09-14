#ifndef SYNTATIC_AUTOMATA_H
#define SYNTATIC_AUTOMATA_H

#include "grammar.h"

#include <src/common/token.h>

#include <list>

#include <functional>

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

  list<int> _stateStack;
  
  list<Symbol> _symbolStack;

  function<Token<TerminalT>()> _getNextToken;

  Grammar<NonTerminalT,TerminalT> _grammar;

  /* o autômato recebe uma uma gramática e uma função getToken*/
  Automata(Grammar<NonTerminalT,TerminalT> &grammar, const function<Token<TerminalT>()> &getToken):
  /* e já gera a tabela de ações/goto -> processo demorado */
  _table(grammar.createTable()),

  /* inicialmente possuo o estado 0 na pilha */
  _stateStack({0}),
  _grammar(grammar),
  
  _getNextToken(getToken)
  {
  }

  void run()
  {
    /* pega o primeiro token */
    Token<TerminalT> token(_getNextToken());

    while (true) {
      /* antes de tudo, minha pilha de símbolos vale */
      cerr << "o topo estados é "
           << _stateStack.back()
           << " e símbolos: ";
      for (auto i(_symbolStack.begin()); i != _symbolStack.end(); i++) {
        cerr << _grammar._symbolToString(*i) << ", ";
      }
      cerr << endl;

      // Ação a ser executada: Reduce, error, goto, accept
      typename LR1Table::iterator action(_table.find(LR1Key(_stateStack.back(),{token.getType()})));

      if (action == _table.end()) {
        cerr << "Erro no parser!" << endl;
        return;
      }

      // aceitação, casou tudo
      if (action->second._action == ACCEPT) {
        cerr << "Aceitou!" << endl;
        return;
      }

      // ação de shift
      if (action->second._action == SHIFT) {
        cerr << "Empilho " << action->second._value 
             << " e " << _grammar._symbolToString(token.getType())
             << endl;
        _stateStack.push_back(action->second._value);
        _symbolStack.push_back(token.getType());
      
        /* pega o próximo token */
        token = _getNextToken();
        continue;
      }

      // ação de reduce
      if (action->second._action == REDUCE) {
        int popSize(_grammar._v[action->second._value]._production.size());

        // o lado esquerdo da produção do reduce 
        Symbol leftSide(_grammar._v[action->second._value]._leftSide);

        cerr << "Preciso remover da pilha pela regra " << action->second._value 
             << " " << popSize 
             << ", size pilha de símbolos  " << _symbolStack.size()
             << " e size estados " << _stateStack.size() << endl;

        typename Grammar<NonTerminalT,TerminalT>::SymbolList symbolList;
        
        /* desempilho x elementos  */
        for (int i(0); i<popSize; i++) {
          symbolList.push_back(_symbolStack.back());
          _symbolStack.pop_back();
          _stateStack.pop_back();
        }

        cerr << "reduzi: ";
        for (auto i(symbolList.rbegin()); i != symbolList.rend(); i++) {
          cerr << _grammar._symbolToString(*i) << ", ";
        }
        cerr << " para " << _grammar._symbolToString(leftSide) << endl;

        /* insere na pilha o lado esquerdo (A) da produção */
        _symbolStack.push_back(leftSide);

        cerr << "Inseri " << _grammar._symbolToString(token.getType()) << endl;

        /* insere na pilha o GOTO[topo,A]. TODO: pode dar erro?  */ 
        _stateStack.push_back(_table[LR1Key(_stateStack.back(),leftSide)]._value);
      }
    } 
  }
};
}
#endif
