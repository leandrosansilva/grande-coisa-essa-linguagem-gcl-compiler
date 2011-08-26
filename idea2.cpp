#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <functional>
#include <set>

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

  bool operator<(const Symbol &other) const
  {
    bool t(other.isTerminal() && isTerminal() && _lexema < other._lexema);
    bool n(other.isNonTerminal() && isNonTerminal() && _nT < other._nT);

    /* se os dois são diferentes, terminal é sempre menor que não terminal */
    bool d(other.isNonTerminal() && isTerminal() && true);

    return t || n || d;
  }

  bool operator>(const Symbol &other) const
  {
    bool t(other.isTerminal() && isTerminal() && _lexema > other._lexema);
    bool n(other.isNonTerminal() && isNonTerminal() && _nT > other._nT);

    /* se os dois são diferentes, terminal é sempre menor que não terminal */
    bool d(other.isTerminal() && isNonTerminal() && true);

    return t || n || d;
  }

  bool operator!=(const Symbol &other) const
  {
    return !(*this == other);
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

  bool operator<(const Item &other) const
  {
    if ((_rule * 10 + _dot) < (other._rule * 10 + other._dot)) {
      return true;
    }
    
    return _s < other._s;
  }

  bool operator>(const Item &other) const
  {
    if ((_rule * 10 + _dot) < (other._rule * 10 + other._dot)) {
      return true;
    }
    
    return _s < other._s;
  }

  bool operator==(const Item &other) const
  {
    return _rule == other._rule && _dot == other._dot && _s == other._s;
  }

  bool operator!=(const Item &other) const
  {
    return !(*this == other);
  }
};

template<typename K1, typename K2>
struct MultiKey
{
  K1 _k1;
  K2 _k2;
  MultiKey(const K1 &k1, const K2 &k2): _k1(k1), _k2(k2)
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

    for(int iId(0); iId < s.size(); iId++) {
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
          /* adiciona o novo item, mas com o ponto no começo */
          s.push_back(Item(i,0,Symbol("a")));
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
      }
    }
    return closure(j);
  }

  void printSetOfItems(const SetOfItems &s)
  {
    for_each(s.begin(), s.end(),[](const Item &item){
      cout << "<" << item._rule << "," << item._dot << ">" << endl;
    });
  }
  
  void canonical(const SetOfItems &s)
  {
    vector<SetOfItems *> t;

    t.push_back(new SetOfItems(closure({{0,0,{""}}})));

    /* mapa onde a chave é uma estrutura <índice da closure,símbolo do goto> 
     * e o valor é o índice da closure resultado da transição
    */
    map<MultiKey<int,Symbol>,int> e;

    /* guarda os items que eu já usei para fazer goto e para onde levam*/
    map<Item,SetOfItems *> usedItems;

    /* s guarda o índice da closure corrente */
    for (int s(0); s < t.size(); s++) {

      for (SetOfItems::iterator item(t[s]->begin()); item != t[s]->end(); item++) {

        // Se o item já foi usado
        if (usedItems.find(*item) != usedItems.end()) {
          //cout << "Já usado" << endl;
          continue;
        } 

        cout << endl << "analysing <" << item->_rule << "," << item->_dot << ">" << endl;

        usedItems[*item] = NULL;

        // para cada cara depois do ponto da regra em questão, faz o goto dele
        SetOfItems j(goTo(*(t[s]),_v[item->_rule]._production[item->_dot]));

        /* se o goto aplicado no elemento resultou num novo conjunto, adiciona */
        if (j.size()) {
          cout << "closure " << s << ":" << endl;

          printSetOfItems(j);

          // se não achou a closure na lista de closures, a adiciona
          t.push_back(new SetOfItems(j));
        }
      }
    }
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


bool TEST(const bool ret, const string &msg)
{
  cout << msg << ": " << (ret ? "TRUE" : "FALSE") << endl << endl;
}

void testSymbol()
{
  Symbol s1(E);
  Symbol s2(T);

  Symbol s3("primeiro");
  Symbol s4("segundo");

  TEST(s1 < s2, "TRUE");
  TEST(s3 < s4, "TRUE");
  
  TEST(s1 > s2, "FALSE");
  TEST(s3 > s4, "FALSE");
 
  TEST(s1 > s4, "TRUE");
  TEST(s2 < s4, "FALSE");

  TEST(s2 == s2, "TRUE");
  TEST(s3 != s3, "FALSE");
}

void testItem()
{
  Item i1(2,2,Symbol("a"));
  Item i2(2,2,Symbol("b"));
  Item i3(2,2,Symbol(E));
  Item i4(2,3,Symbol("b"));
  Item i5(3,2,Symbol("b"));

  TEST(i1 < i2,"TRUE");
  TEST(i1 != i1,"FALSE");
  TEST(i1 < i3,"TRUE");
}

int main(int argc, char **argv)
{
  //testItem();
 
  g.canonical({{0,0,{""}}});

  return 0;
}
