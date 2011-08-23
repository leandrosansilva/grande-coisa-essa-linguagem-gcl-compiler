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

      cout << "tamanho da lista " << "(" << iId << "<" <<  s.size() << ") ";
      cout << "em mãos item " << "<" << curItem._rule << "," << curItem._dot << ">" << endl;

      /* se o ponto do item não se encntra num não-terminal, nada a ser feito */
      if (!_v[curItem._rule]._production[curItem._dot].isNonTerminal()) {
        cout << "saí com terminal na posição " << curItem._dot << " na regra " << curItem._rule << endl;
        continue;
      }

      for (int i(0); i < _v.size(); i++) {
        cout << endl;

        /* Se já foi adicionado, faço nada */
        if (used[i]) {
          cout << "saí com já usado em " << i << endl;
          continue;
        }

        if (_v[i]._leftSide == _v[curItem._rule]._production[curItem._dot]._nT) {
          cout << "adicionei a regra nº " << i << endl; 
          used[i] = true;
          s.push_back(Item(i,curItem._dot,Symbol("a")));
        } else {
          cout << "não adicionei a regra nº " << i << endl; 
        }
      }
    }

    return s;
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
  SetOfItems s(g.closure({{0,0,{""}}}));

  cout << "result:" << endl;
    
  for_each(s.begin(),s.end(),[](const Item &item){
    cout << "<" << item._rule << "," << item._dot << ">" << endl;
  });
  
  return 0;
}
