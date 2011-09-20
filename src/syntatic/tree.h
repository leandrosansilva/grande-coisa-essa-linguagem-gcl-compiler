#ifndef TREE_H
#define TREE_H

#include <map>
#include <list>

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
  
  /* uma árvore é um mapa onde a chave é um 
   * símbolo que leva numa árvore 
  */
  typedef map<SymbolT, Tree*> TreeMap;

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

  template<typename CallBack>
  string toString(const CallBack &symbolToString) const
  {
    string out;
    out += "(";
    if (isLeaf()) {
      out += "\"" + _token.getLexema() + "\"";
    } else {
      stringstream ss;
      for (auto i(_tree.begin()); i!= _tree.end(); i++) {
        ss << symbolToString(i->first) << ": " << i->second->toString(symbolToString) << ",";
      }
      out += ss.str();
    }
    out += ")";
    return out;
  }
  
};

}
#endif

