#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <functional>
#include <set>
#include <list>

using namespace std;

typedef enum
{
  EL,E, T, F
} NonTerminal;

map<NonTerminal,string> NonTerminalMap 
{
  {EL,"EL"},
  {E,"E"},
  {T,"T"},
  {F,"F"}
};

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

  string toString() const 
  {
    if (isTerminal()) {
      return _lexema;
    }

    return NonTerminalMap[_nT]; 
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
  _s("")
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
    if ((_rule * 10 + _dot) > (other._rule * 10 + other._dot)) {
      return true;
    }
    
    return _s > other._s;
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

typedef vector<Item> SetOfItems;

typedef pair<list<SetOfItems *>,map<Item,SetOfItems *>> CanonicalPair;

struct Grammar
{
  vector<Rule<NonTerminal,SymbolList,int>> _v;

  Grammar(const vector<Rule<NonTerminal,SymbolList,int>> &v):_v(v)
  {
  }

  void printItem(const Item &item)
  {
    // imprime o left side
    cout << NonTerminalMap[_v[item._rule]._leftSide] << " -> ";
    for (int i(0); i<_v[item._rule]._production.size(); i++) {
      if (i == item._dot) {
        cout << ". ";
      }
      cout << _v[item._rule]._production[i].toString() << " ";
    }

    if (item._dot == _v[item._rule]._production.size()) {
      cout << ". ";
    }
    cout << endl;
  }

  void printSetOfItems(const SetOfItems &s)
  {
    for (int i(0); i< s.size(); i++) {
      printItem(s[i]);
    }
  }

  SetOfItems closure(const SetOfItems &soi)
  {
    /* copia I em J */
    SetOfItems s(soi);

    set<Item> used;

    for(int iId(0); iId < s.size(); iId++) {
      Item curItem(s[iId]);

      /* se o ponto do item não se encntra num não-terminal, nada a ser feito */
      if (!_v[curItem._rule]._production[curItem._dot].isNonTerminal()) {
        continue;
      }

      for (int i(0); i < _v.size(); i++) {

        if (_v[i]._leftSide == _v[curItem._rule]._production[curItem._dot]._nT) {

          Item item(i,0,Symbol(""));

          /* Se já foi usado, aio */
          if (used.find(item) != used.end()) {
            continue;
          }

          /* adiciona o novo item, mas com o ponto no começo */
          s.push_back(item);

          used.insert(item);
        }
      }
    }

    return s;
  }

  SetOfItems goTo(const SetOfItems &items,const Symbol &x)
  {
    SetOfItems j;
    for (SetOfItems::const_iterator it(items.begin()); it != items.end(); it++) {
      if (_v[it->_rule]._production[it->_dot] == x) {
        j.push_back(Item(it->_rule,it->_dot+1,Symbol("")));
      }
    }
    
    return closure(j);
  }

  CanonicalPair canonical(const SetOfItems &s)
  {
    list<SetOfItems *> f, t({new SetOfItems(closure({{0,0,{""}}}))});

    /* guarda os items que eu já usei para fazer goto e para onde levam*/
    map<Item,SetOfItems *> usedItems;

    /* enquanto houver elementos na lista, remove e joga na lista final */
    while (t.size()) {

      map<Symbol,bool> usedSymbols;

      /* TODO: isto não precisa acontecer para todos os símbolos da gramática, 
       * mas só para os do set
      */
      for (int i(0); i< _v.size(); i++) {
        if (usedSymbols.find(Symbol(_v[i]._leftSide)) == usedSymbols.end()) {
          usedSymbols[Symbol(_v[i]._leftSide)] = false;
        }
      }

      /* pega o primeiro da lista e o remove */
      SetOfItems *s(t.front());
      f.push_back(s);
      t.pop_front();

      cout << "inserindo: " << endl;
      printSetOfItems(*s);
      cout << endl;

      for (SetOfItems::iterator item(s->begin()); item != s->end(); item++) {

        // Se o item já foi usado
        if (usedItems.find(*item) != usedItems.end()) {
          continue;
        }

        /* Se já foi feito o goto num símbolo, sai  */
        if (usedSymbols[_v[item->_rule]._production[item->_dot]]) {
          continue;  
        }

        usedSymbols[_v[item->_rule]._production[item->_dot]] = true;
        
        // para cada cara depois do ponto da regra em questão, faz o goto dele
        SetOfItems j(goTo(*s,_v[item->_rule]._production[item->_dot]));

        /* se o goto aplicado no elemento resultou num novo conjunto, adiciona */
        if (j.size()) {

          //cout << endl << "goto({";
          //printSetOfItems(*s);
          //cout << "}, " << _v[item->_rule]._production[item->_dot].toString() << " ) -> ";
          //printSetOfItems(j);

          SetOfItems *dst(new SetOfItems(j));

          t.push_back(dst);

          usedItems[*item] = dst;
        }
      }
    }
    return {f,usedItems};
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
  Item i6(2,2,Symbol(EL));

  TEST(i1 < i2,"TRUE");
  TEST(i1 != i1,"FALSE");
  TEST(i1 < i3,"TRUE");
  TEST(i3 > i1,"TRUE");
  TEST(i3 == i1,"FALSE");
  TEST(i6 == i3,"FALSE");
  TEST(i6 < i3,"TRUE");
}

int main(int argc, char **argv)
{
  CanonicalPair c(g.canonical({{0,0,{""}}}));

  cout << "nº de closures: " << c.first.size() << " e de itens: " << c.second.size() << endl;

  return 0;
}
