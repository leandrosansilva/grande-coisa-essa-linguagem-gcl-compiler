#ifndef SYNTATIC_AUTOMATA_H
#define SYNTATIC_AUTOMATA_H

#include "grammar.h"

#include "tree.h"

#include <src/common/token.h>

#include <list>

#include <functional>

using namespace std;

using namespace Common;

namespace Syntatical {

template <typename TokenTypeT, typename SymbolT>
Tree<TokenTypeT,SymbolT> createTree(const SymbolT &head, const list<Tree<TokenTypeT,SymbolT>> &list) 
{
  typename Tree<TokenTypeT,SymbolT>::TreeMap treeMap;

  /* FIXME: preencher a árvore com as subárvores */
  for (auto i(list.begin()); i != list.end(); i++) {
    treeMap[i->getHead()] = new Tree<TokenTypeT,SymbolT>(*i);
  }
  
  /* retorna uma árvore  */
  return {head,treeMap};
}

template<typename NonTerminalT, typename TerminalT>
struct Automata
{
  /* FIXME: isto está feio. As duas classes ficam muito desconexas assim */
  typedef typename Grammar<NonTerminalT,TerminalT>::Symbol Symbol;

  typedef typename Grammar<NonTerminalT,TerminalT>::LR1Table LR1Table;

  typedef typename Grammar<NonTerminalT,TerminalT>::LR1Key LR1Key;
  
  LR1Table _table;

  list<int> _stateStack;
  
  /* uma pilha (in fact a list) de árvores */
  list<Tree<TerminalT,Symbol>> _symbolStack;

  /* a árvore final */
  Tree<TerminalT,Symbol> _tree;

  Tree<TerminalT,Symbol> getTree() const
  {
    return _tree;
  }

  function<Token<TerminalT>()> _getNextToken;

  Grammar<NonTerminalT,TerminalT> _grammar;

  /* o autômato recebe uma uma gramática e uma função getToken */
  Automata(Grammar<NonTerminalT,TerminalT> &grammar, const function<Token<TerminalT>()> &getToken):

  /* e já gera a tabela de ações/goto -> FIXME: processo demorado */
  _table(grammar.createTable()),

  /* inicialmente possuo o estado 0 na pilha de estados */
  _stateStack({0}),
  _grammar(grammar),
  
  _getNextToken(getToken)
  {
  }

  bool parse() 
  {
    /* pega o primeiro token */
    Token<TerminalT> token(_getNextToken());

    /* while true é do mal */
    while (true) {
      // Ação a ser executada: Reduce, error, goto, accept
      typename LR1Table::iterator action(_table.find(LR1Key(_stateStack.back(),{token.getType()})));

      /* estado de erro! */
      if (action == _table.end()) {
        cerr << "Erro no parser!" << endl;
        cerr << "Topo estado: " << _stateStack.back() << " e topo symbol"
             << _grammar._symbolToString(_symbolStack.back()) << endl;
        return false;
      }

      // aceitação, casou tudo
      if (action->second._action == ACCEPT) {
        cerr << "Aceitou!" << endl;
  
        /* o próprio topo da pilha */
        _tree = _symbolStack.back();

        return true;
      }

      // ação de shift
      if (action->second._action == SHIFT) {
        cerr << "Empilho " << action->second._value 
             << " e " << _grammar._symbolToString(token.getType())
             << endl;
        _stateStack.push_back(action->second._value);
        _symbolStack.push_back(token);
      
        // pega o próximo token
        token = _getNextToken();

        cerr << "Li o token " << _grammar._symbolToString(token.getType()) << endl;
        continue;
      }

      // ação de reduce
      if (action->second._action == REDUCE) {
        int popSize(_grammar._v[action->second._value]._production.size());

        // conjunto que será passado para a ação semântica
        list<Tree<TerminalT,Symbol>> symbolList;

        // desempilho x elementos
        for (int i(0); i<popSize; i++) {
          symbolList.push_back(_symbolStack.back());
          _symbolStack.pop_back();
          _stateStack.pop_back();
        }

        
        // o lado esquerdo da produção do reduce 
        Symbol leftSide(_grammar._v[action->second._value]._leftSide);

        // a árvore que será empilhada
        Tree<TerminalT,Symbol> pushedTree(createTree<TerminalT,Symbol>(leftSide,symbolList));

        int topState(_stateStack.back());

        TableAction gAction(_table[LR1Key(_stateStack.back(),leftSide)]);

        int gDst(gAction._value);

        
        cerr << "reduzi: ";
        for (auto i(symbolList.rbegin()); i != symbolList.rend(); i++) {
          cerr << _grammar._symbolToString(*i) << ", ";
        }

        cerr << " para " << _grammar._symbolToString(leftSide) 
             << " e empilhei estado " << gDst << endl;

        // para que estado devo fazer GOTO?
        cerr << "TABLE[" << topState << ","
             << _grammar._symbolToString(leftSide) << "] == ";

                cerr << "<" << _table.actionToString(gAction._action) 
               << "," << gDst << ">" << endl;


        // insere na pilha o lado esquerdo (A) da produção
        _symbolStack.push_back(pushedTree);

        // insere na pilha o GOTO[topo,A]. TODO: pode dar erro?
        _stateStack.push_back(gDst);
        
      }
    } 
  }
};
}
#endif
