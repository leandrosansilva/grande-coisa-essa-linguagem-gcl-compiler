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
  EL,E, T, F,
  
  D,K,B,H
} NonTerminal;

map<NonTerminal,string> NonTerminalMap 
{
  {EL,"EL"},
  {E,"E"},
  {T,"T"},
  {F,"F"},
  {D,"D"},
  {K,"K"},
  {B,"B"},
  {H,"H"}
};

struct Symbol
{
  typedef enum { TERMINAL, NONTERMINAL, ENDMARK, EMPTY} Type;

  Type _type;

  string _lexema;

  NonTerminal _nT;

  bool isEndMark() const
  {
    return _type == ENDMARK;
  }

  bool isEmpty() const 
  {
    return _type == EMPTY;
  }

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

  /* símbolo vazio */
  Symbol(): _type(EMPTY)
  {
  }

  string toString() const 
  {
    if (isTerminal()) {
      return _lexema;
    }
  
    if (isEmpty()) {
      return "<EMPTY>";
    }

    return NonTerminalMap[_nT]; 
  }

  bool operator<(const Symbol &other) const
  {
    bool t(other.isTerminal() && isTerminal() && _lexema < other._lexema);
    bool n(other.isNonTerminal() && isNonTerminal() && _nT < other._nT);

    /* se os dois são diferentes, terminal é sempre menor que não terminal */
    bool d(other.isNonTerminal() && isTerminal());

    /* se é vazio, é menor que qualquer coisa, a não ser que esta outra coisa seja vazio */
    bool e(isEmpty() && !other.isEmpty());

    return t || n || d || e;
  }

  bool operator>(const Symbol &other) const
  {
    return (!((*this) < other || (*this) == other));
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

    bool e(isEmpty() && other.isEmpty());

    return t || n || e;
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

  SymbolList first(const Symbol &symbol)
  {
    if (symbol.isTerminal() || symbol.isEmpty()) {
      return {symbol};
    }

    SymbolList f;

    /* para cada produção do símbolo, vai adicionando o first dele em f */
    for (int i(0); i<_v.size(); i++) {
      /* é uma produção do símbolo em questão  */  
      if (_v[i]._leftSide == symbol._nT) {

        /* se o tamanho da produção é 0, significa que produz vazio */
        if (!_v[i]._production.size()) {
          /* adiciona vazio na lista e sai */
          f.push_back({});
          break;
        }

        for (int j(0); j<_v[i]._production.size(); j++) {
          /* pega o first do elemento em questão.  */
          SymbolList pF(first(_v[i]._production[j]));

          /* TODO: achar um merge */
          for (int k(0); k < pF.size(); k++) {
            f.push_back(pF[k]);
          }

          /* não achei vazio no first. Logo, devo parar */
          if (std::find(pF.begin(), pF.end(),Symbol()) == pF.end()) {
            break;
          }
        }
      }
    }

    return f;
  }

  void printItem(const Item &item)
  {
    // imprime o left side
    cout << NonTerminalMap[_v[item._rule]._leftSide] << " -> ";

    // imprime a produção
    for (int i(0); i<_v[item._rule]._production.size(); i++) {
      if (i == item._dot) {
        cout << ". ";
      }
      cout << _v[item._rule]._production[i].toString() << " ";
    }

    // no caso de o ponto estar no final
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

      /* comparo o item com todas as regras da gramática  */
      for (int i(0); i < _v.size(); i++) {

        /* só aplico às regras cuja símbolo da esquerda seja igual ao que tenho em mãos */
        if (_v[i]._leftSide == _v[curItem._rule]._production[curItem._dot]._nT) {

          SymbolList f(first({}));

          /* pra cada símbolo achado em first, se não o usei, adiciono na closure */
          for (int j(0); j<f.size();j++) {
            Item item(i,0,f[j]);

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
    }

    return s;
  }

  SetOfItems goTo(const SetOfItems &items,const Symbol &x)
  {
    SetOfItems j;

    for (SetOfItems::const_iterator it(items.begin()); it != items.end(); it++) {
      if (_v[it->_rule]._production[it->_dot] == x) {
        j.push_back(Item(it->_rule,it->_dot+1,it->_s));
      }
    }
    
    return closure(j);
  }

  CanonicalPair canonical(const SetOfItems &s)
  {
    list<SetOfItems *> f, t({new SetOfItems(closure({{0,0,{}}}))});

    /* guarda os items que eu já usei para fazer goto e para onde levam*/
    map<Item,SetOfItems *> usedItems;

    /* enquanto houver elementos na lista, remove e joga na lista final */
    while (t.size()) {

      /* pega o primeiro da lista, remove-o e insere na lista final */
      SetOfItems *s(t.front());
      f.push_back(s);
      t.pop_front();

      cout << "inserindo: " << endl;
      printSetOfItems(*s);
      cout << endl;

      /* nesta iteração, quais símbolos já foram usados? */
      map<Symbol,bool> usedSymbols;

      /* TODO: isto não precisa acontecer para todos os símbolos da gramática, 
       * mas só para os do set
      */
      for (int i(0); i < _v.size(); i++) {
        if (usedSymbols.find(Symbol(_v[i]._leftSide)) == usedSymbols.end()) {
          usedSymbols[Symbol(_v[i]._leftSide)] = false;
        }
      }

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


bool TEST(const bool ret, const string &msg)
{
  cout << msg << ": " << (ret ? "TRUE" : "FALSE") << endl;
}

void testSymbol()
{
  Symbol s1(E);
  Symbol s2(T);

  /* dois vazios */
  Symbol v1, v2;

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

  TEST(v1 == v2,"TRUE");
  TEST(v1 != v2,"FALSE");
  TEST(v1 == v1,"TRUE");
  TEST(v1 == s1,"FALSE");
  TEST(s1 == v1,"FALSE");
  TEST(s1 != v1,"TRUE");

  SymbolList list { {"a"}, {E}, {}, {"b"}, {EL}, {E}, {"zaza"}, {}, {"a"}, {T}, {}, {}};

  std::sort(list.begin(), list.end());
    
  for (int i(0); i< list.size(); i++) {
    cout << list[i].toString() << ", ";
  }
  cout << endl;
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

void testFirst()
{
  Grammar g({
    {F,{{D}},1},
    {F,{{K}},1},
    {D,{{"a"},{B}},1},
    {D,{{Symbol()}},1},
    {K,{{"c"},{H}},1},
    {H,{{"h"}},1}
  });

  SymbolList a(g.first({F}));

  for (int i(0); i< a.size(); i++) {
    cout << a[i].toString() << ", ";
  }
  cout << endl;
}

Grammar g ({
  {EL,{{E}},1},
  {E,{{E},{"+"},{T}},1},
  {E,{{T}},1},
  {T,{{T},{"*"},{F}},1},
  {T,{{F}},1},
  {F,{{"("},{E},{")"}},1},
  {F,{{"id"}},1}
});

void testClosure()
{
  SetOfItems s(g.closure({{0,0,{}}}));
  
  g.printSetOfItems(s);
}


int main(int argc, char **argv)
{
  //CanonicalPair c(g.canonical({{0,0,{}}}));

  //cout << "nº de closures: " << c.first.size() << " e de itens: " << c.second.size() << endl;

  testFirst();

  //testSymbol();

  return 0;
}
