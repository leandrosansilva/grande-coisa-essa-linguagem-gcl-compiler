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
  
  /* construtor que recebe um token */
  Tree(const Token<TokenTypeT> &token):
  _token(token),
  _type(LEAF)
  {
  }
  
  /* construtor que recebe o nome e o valor de uma árvore */
  Tree(const SymbolT &key, TreeMap tree):
  _tree(tree),
  _type(TREE)
  {
  }

  bool isLeaf() const
  {
    return _type == LEAF;
  }

  string toString() const
  {
    string out;
    out += "(";
    if (isLeaf()) {
      out += "<leaf>";
    } else {
      stringstream ss;
      for (auto i(_tree.begin()); i!= _tree.end(); i++) {
        ss << i->first << ": " << i->second->toString() << ",";
      }
      out += ss.str();
    }
    out += ")";
    return out;
  }
  
};

}
#endif

