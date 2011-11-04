#ifndef SEMANTICANALYSER_H
#define SEMANTICANALYSER_H

#include <syntatic/tree.h>

namespace Semantic {

/*
 * Para cada nó não-folha lido, chama uma função que processa o nó em questão. 
 * Para a análise, há somente retorno true/false
 * Toda função deve ter conhecimento da função atual (descrição na tabela de símbolos)
 * bem como das tabelas de tipos e da de variáveis
 * 
 * Por exemplo, numa atribuição, a função tratadora deve olhar se o 
 * tipo do parâmetro da esquerda e o da direita são iguais, retornando um erro caso sejam diferentes
 *
 * Numa instrução de retorno, deve-ve verificar se o tipo do 
 * mapa[FunctionCall] = new FunctionCallAnalyser(tabelaDeVariaveis,TabelaDeFuncoes,TabelaDeTipos)
 *
*/

template<typename NonTerminalT,typename TokenTypeT, typename CodeType = string>
struct Analyser {

  struct NodeAnalyser;

  map<NonTerminalT,NodeAnalyser *> _aMap;

  typedef typename Syntatical::Grammar<NonTerminalT,TokenTypeT>::Symbol Symbol;

  Syntatical::Tree<TokenTypeT,Symbol> &_tree;

  Analyser(Syntatical::Tree<TokenTypeT,Symbol> &tree, const map<NonTerminalT,NodeAnalyser *> &aMap):
  _tree(tree),
  _aMap(aMap)
  {
    // coloco uma referência ao mapa nó-objeto em cada um dos objetos novos
    for (auto i(_aMap.begin()); i != _aMap.end(); i++) {
      i->second->setAMap(&_aMap);
    }
  }

  CodeType getCode()
  {
    auto found(_aMap.find(_tree.getHead()._nonTerminal));
    
    if (found == _aMap.end()) {
      throw string("node analyser not found!");
    }
  
    return found->second->getCode(_tree); 
  }

  struct NodeAnalyser
  {
    map<NonTerminalT,NodeAnalyser *> *_aMap;

    NodeAnalyser():_aMap(NULL)
    {
    }

    void setAMap(map<NonTerminalT,NodeAnalyser *> *a)
    {
      _aMap = a;
    }

    virtual CodeType getCode(Syntatical::Tree<TokenTypeT,Symbol> &t) = 0;
    
    virtual NodeAnalyser *getAnalyser(Syntatical::Tree<TokenTypeT,Symbol> &t)
    {
      auto found(_aMap->find(t.getHead()._nonTerminal));

      if (found == _aMap->end()) {
        throw string("node analyser not found!");
      }

      return found->second;
    }
  };
};

}

#endif
