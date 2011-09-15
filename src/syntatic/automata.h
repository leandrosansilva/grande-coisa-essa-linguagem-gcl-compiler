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
      // Ação a ser executada: Reduce, error, goto, accept
      typename LR1Table::iterator action(_table.find(LR1Key(_stateStack.back(),{token.getType()})));

      if (action == _table.end()) {
        cerr << "Erro no parser!" << endl;
        cerr << "Topo estado: " << _stateStack.back() << " e topo symbol"
             << _grammar._symbolToString(_symbolStack.back()) << endl;
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

        cerr << "Li o token " << _grammar._symbolToString(token.getType()) << endl;
        continue;
      }

      // ação de reduce
      if (action->second._action == REDUCE) {
        int popSize(_grammar._v[action->second._value]._production.size());

        /* conjunto que será passado para a ação semântica  */
        typename Grammar<NonTerminalT,TerminalT>::SymbolList symbolList;

        /* desempilho x elementos  */
        for (int i(0); i<popSize; i++) {
          symbolList.push_back(_symbolStack.back());
          _symbolStack.pop_back();
          _stateStack.pop_back();
        }

        // o lado esquerdo da produção do reduce 
        Symbol leftSide(_grammar._v[action->second._value]._leftSide);

        /* para que estado devo fazer GOTO? */
        cerr << "TABLE[" << _stateStack.back() << ","
             << _grammar._symbolToString(leftSide) << "] == ";

        TableAction gAction(_table[LR1Key(_stateStack.back(),leftSide)]);

        int gDst(gAction._value);
        cerr << "<" << _table.actionToString(gAction._action) 
               << "," << gDst << ">" << endl;
        
        cerr << "reduzi: ";
        for (auto i(symbolList.rbegin()); i != symbolList.rend(); i++) {
          cerr << _grammar._symbolToString(*i) << ", ";
        }

        cerr << " para " << _grammar._symbolToString(leftSide) 
             << " e empilhei estado " << gDst << endl;

        /* insere na pilha o lado esquerdo (A) da produção */
        _symbolStack.push_back(leftSide);

        /* insere na pilha o GOTO[topo,A]. TODO: pode dar erro?  */
        _stateStack.push_back(gDst);
      }
    } 
  }
};
}
#endif
