#ifndef TREE_H
#define TREE_H

#include <vector>

#include <tuple>

#include <sstream>

#include <string>

#include <src/common/token.h>

using namespace std;

using namespace Common;

namespace Syntatical {

/* Uma árvore funciona assim: se foi construída a partir de um token
 * então é um nó folha
*/

/* um tipo de token, síbolo e   */
template<typename TokenTypeT, typename SymbolT>
struct Tree
{
  /* qual o símbolo do nó? */
  SymbolT _head;

  /* uma subárvore é uma tupla com uma "label" e uma árvore */
  typedef tuple<SymbolT, Tree*> TreeChild;
  
  typedef vector<TreeChild> TreeMap;

  /* uma árvore pode ser folha ou ter subárvores */
  typedef enum {
    LEAF,
    TREE
  } Type;

  /* de qual tipo é a árvore? */
  Type _type;

  Token<TokenTypeT> _token;
  
  TreeMap _tree;

  /* construtor vazio, só pra compilar */
  Tree()
  {
  }
  
  /* construtor que recebe um token */
  Tree(const Token<TokenTypeT> &token):
  _token(token),
  _type(LEAF),
  _head(token.getType())
  {
  }
  
  /* construtor que recebe o nome e o valor de uma árvore */
  Tree(const SymbolT &key, TreeMap tree):
  _tree(tree),
  _type(TREE),
  _head(key)
  {
  }

  bool isLeaf() const
  {
    return _type == LEAF;
  }

  /* retorna qual o símbolo do nó */
  SymbolT getHead() const
  {
    return _head;
  }

  /* para poder ser usado como Symbol */
  operator SymbolT() const
  {
    return getHead();
  }

  void dispose()
  {
    for (auto t(_tree.begin()); t != _tree.end(); t++) {
      get<1>(*t)->dispose();
      delete get<1>(*t);
    }
  }

  template<typename CallBack>
  string toString(const CallBack &symbolToString, int indent = 0) const
  {
    string out("\n");

    // coloca a indentação
    for (int spaces(0); spaces < indent; spaces++) {
      out += ".";
    }

    out += symbolToString(getHead()) + ": ";

    if (isLeaf()) {
      out += "\"" + _token.getLexema() + "\"";
    } else {
      stringstream ss;

      int i(0);

      int size(_tree.size());

      /* imprime todas as árvores, em ordem, menos a última */
      for (;i < size - 1; i++) {
        ss << get<1>(_tree[i])->toString(symbolToString,indent+1);
      }

      /* imprime a última árvore */
      if (size) {
        ss << get<1>(_tree[i])->toString(symbolToString,indent+1);
      }
      
      out += ss.str();
    }

    return out;
  }

  template<typename CallBack>
  void generateGraph(const CallBack &symbolToString) const
  {
    int count(0);

    cout << "digraph MyGrammar { " << endl;

    generateSubGraph<CallBack>(symbolToString,count);

    cout << "}" << endl;
  }

  template<typename CallBack>
  void generateSubGraph(const CallBack &symbolToString, int &count) const
  {
    int myCount(count);
  
    /* */
    if (isLeaf()) {
      cout << "  c" << count << " [shape=oval,style=filled,color=\"#49FF61\",label=\"" << _token.getLexema() << "\"];" << endl;
      return;
    }

    cout << "  c" << count << " [shape=rect,label=\"" << symbolToString(_head) << "\"];" << endl;

    int size(_tree.size());

    /* imprime todas as árvores, em ordem, menos a última */
    for (int i(0); i < size; i++) {
      count++;
      cout << "  c" << myCount << " -> " << "c" << count << ";" << endl;
      get<1>(_tree[i])->generateSubGraph(symbolToString,count);
    }
  }
};

}
#endif
