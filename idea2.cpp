#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;

typedef enum
{
  EL,E, T, F
} NonTerminal;

struct Symbol
{
  typedef enum { TERMINAL, NONTERMINAL} Type;

  Type _type;

  string _lexema;

  NonTerminal _nT;

  bool isTerminal() const
  {
    return _type == TERMINAL;
  }

  bool isNonTerminal() const
  {
    return _type == NONTERMINAL;
  }

  Symbol(const string &tok): _type(TERMINAL),_lexema(tok)
  {
  }
  
  Symbol(const NonTerminal &nT): _type(NONTERMINAL),_nT(nT)
  {
  }

  /* operadores de comparação */
  bool operator==(const Symbol &other) const
  {
    /* TODO: reescrever com operador ternário */
    bool t(other.isTerminal() && isTerminal() && _lexema == other._lexema);
    bool n(other.isNonTerminal() && isNonTerminal() && _nT == other._nT);

    return t || n;
  }
};

typedef vector<Symbol> SymbolList;

template<typename LeftSide,typename Production, typename Action>
struct Rule
{
  LeftSide _leftSide; 
  Production _production; 
  Action _action;

  Rule(const LeftSide &ls, const Production &prod, const Action &act):
  _leftSide(ls),
  _production(prod),
  _action(act)
  {
  }
};

struct Item
{
  int _rule;
  int _dot;
  Symbol _s;
  Item(int ruleNumber,int dot, const Symbol &s):
  _rule(ruleNumber),
  _dot(dot),
  _s(s)
  {
  }
};

typedef vector<Item> SetOfItems;

struct Grammar
{
  vector<Rule<NonTerminal,SymbolList,int>> _v;

  Grammar(const vector<Rule<NonTerminal,SymbolList,int>> &v):_v(v)
  {
  }

  SetOfItems closure(const SetOfItems &soi)
  {
    /* copia I em J */
    SetOfItems s(soi);

    vector<bool> used(int(_v.size()),false);

    /* dos itens que vieram na clusure, defino as regras que já foram usadas */
    for (auto it(s.begin()); it != s.end(); it++) {
      used[it->_rule] = true;
    }

    for(int iId(0);iId < s.size(); iId++) {
      Item curItem(s[iId]);

      /* se o ponto do item não se encntra num não-terminal, nada a ser feito */
      if (!_v[curItem._rule]._production[curItem._dot].isNonTerminal()) {
        continue;
      }

      for (int i(0); i < _v.size(); i++) {
        /* Se já foi adicionado, faço nada */
        if (used[i]) {
          continue;
        }

        if (_v[i]._leftSide == _v[curItem._rule]._production[curItem._dot]._nT) {
          used[i] = true;
          s.push_back(Item(i,curItem._dot,Symbol("a")));
        } else {
        }
      }
    }

    return s;
  }

  SetOfItems goTo(const SetOfItems &items,const Symbol &x)
  {
    SetOfItems j;
    for (auto it(items.begin()); it != items.end(); it++) {
      if (_v[it->_rule]._production[it->_dot] == x) {
        j.push_back(Item(it->_rule,it->_dot+1,Symbol("a")));
      } else {
        cout << "diferentes" << endl;
      }
    }
    return closure(j);
  }
};

Grammar g ({
  {EL,{{E}},1},
  {E,{{E},{"+"},{T}},1},
  {E,{{T}},1},
  {T,{{T},{"*"},{F}},1},
  {T,{{F}},1},
  {F,{{"("},{E},{")"}},1},
  {F,{{"id"}},1}
});

int main(int argc, char **argv)
{
  SetOfItems s(g.goTo(g.closure({{6,1,{""}}}),Symbol("*")));

  cout << "result:" << endl;
    
  for_each(s.begin(),s.end(),[](const Item &item){
    cout << "<" << item._rule << "," << item._dot << ">" << endl;
  });
  
  return 0;
}
