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

typedef vector<Item> ItemList;

typedef set<Symbol> SymbolSet;

typedef pair<list<ItemList *>,map<Item,ItemList *>> CanonicalPair;

struct Grammar
{
  vector<Rule<NonTerminal,SymbolList,int>> _v;

  Grammar(const vector<Rule<NonTerminal,SymbolList,int>> &v):_v(v)
  {
  }

  /* regra e posição que já foi checado */
  set<pair<int,int>> _firstChecked;

  SymbolSet first(const Symbol &symbol)
  {
    if (symbol.isTerminal() || symbol.isEmpty()) {
      return {symbol};
    }

    SymbolSet f;

    /* para cada produção do símbolo, vai adicionando o first dele em f */
    for (int i(0); i<_v.size(); i++) {
      /* é uma produção do símbolo em questão  */  
      if (_v[i]._leftSide == symbol._nT) {

        /* se o tamanho da produção é 0, significa que produz vazio */
        if (!_v[i]._production.size()) {
          /* adiciona vazio na lista e sai */
          f.insert(Symbol());
        }

        for (int j(0); j<_v[i]._production.size(); j++) {
          /* o símbolo no qual vou  */
          Symbol s(_v[i]._production[j]);

          /* isso é basicamente para contornar a recursão à esquerda */
          if (_firstChecked.find({i,j}) == _firstChecked.end()) {
            _firstChecked.insert({i,j});
          } else {
            break;
          }

          /* pega o first do elemento em questão.  */
          SymbolSet pF(first(s));
    
          /* TODO: achar um merge */
          for (auto k(pF.begin()); k != pF.end(); k++) {
            f.insert(*k);
          }

          /* não achei vazio no first. Logo, devo parar */
          if (pF.find(Symbol()) == pF.end()) {
            break;
          }
        }
      }
    }

    return f;
  }

  SymbolSet first(const SymbolList &list)
  {
    SymbolSet f;

    for(auto i(list.begin()); i != list.end(); i++) {

      SymbolSet sF(first(*i));

      for (auto j(sF.begin()); j != sF.end(); j++) {
        f.insert(*j);
      }

      /* se não há vazio no conjunto achado, sai */
      if (sF.find({}) == sF.end()) {
        break;
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

    cout << "la: '" << item._s.toString() << "'" << endl;
  }

  void printItemList(const ItemList &s)
  {
    for (int i(0); i< s.size(); i++) {
      printItem(s[i]);
    }
  }

  template <typename T>
  void printSymbolList(const T &s)
  {
    for (auto i(s.begin()); i != s.end(); i++) {
      cout << i->toString() << ", ";
    }
    cout << endl;
  }
 

  ItemList closure(const ItemList &soi)
  {
    /* copia I em J */
    ItemList s(soi);

    set<Item> used;

    for(int iId(0); iId < s.size(); iId++) {
      Item curItem(s[iId]);

      cout << "com ponto em "  << curItem._dot << " aplicando na regra(" << curItem._rule << "): ";
      printSymbolList(_v[curItem._rule]._production);

      /* concateno todos os símbolos depois do ponto com o lookahead */
      SymbolList withLA(_v[curItem._rule]._production.begin() + curItem._dot ,_v[curItem._rule]._production.end());

      /* concateno o lookahead */
      withLA.push_back(curItem._s);

      /*   */
      SymbolSet f(first(withLA));

      cout << "first res: ";
      printSymbolList(f);

      /* se o ponto do item não se encntra num não-terminal, nada a ser feito */
      if (!_v[curItem._rule]._production[curItem._dot].isNonTerminal()) {
        continue;
      }

      /* comparo o item com todas as regras da gramática  */
      for (int i(0); i < _v.size(); i++) {

        /* só aplico às regras cuja símbolo da esquerda seja igual ao que tenho em mãos */
        if (_v[i]._leftSide != _v[curItem._rule]._production[curItem._dot]._nT) {
          continue;
        }

        /* pra cada símbolo achado em first, se não o usei, adiciono na closure */
        for (auto w(f.begin()); w != f.end();w++) {
          Item item(i,0,*w);

          /* Se já foi usado, vou para o próximo  */
          if (used.find(item) != used.end()) {
            continue;
          }

          cout << "inserindo ";
          printItem(item);
      
          /* adiciona o novo item, mas com o ponto no começo */
          s.push_back(item);

          used.insert(item);
        }
      }
    }

    return s;
  }

  ItemList goTo(const ItemList &items,const Symbol &x)
  {
    ItemList j;

    for (auto it(items.begin()); it != items.end(); it++) {
      if (_v[it->_rule]._production[it->_dot] == x) {
        j.push_back(Item(it->_rule,it->_dot+1,it->_s));
      }
    }

    return closure(j);
  }

  CanonicalPair canonical(const ItemList &s)
  {
    list<ItemList *> f, t({new ItemList(closure({{0,0,{}}}))});

    /* guarda os items que eu já usei para fazer goto e para onde levam*/
    map<Item,ItemList *> usedItems;

    /* enquanto houver elementos na lista, remove e joga na lista final */
    while (t.size()) {

      /* pega o primeiro da lista, remove-o e insere na lista final */
      ItemList *s(t.front());
      f.push_back(s);
      t.pop_front();

      cout << "inserindo: " << endl;
      printItemList(*s);
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

      for (ItemList::iterator item(s->begin()); item != s->end(); item++) {

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
        ItemList j(goTo(*s,_v[item->_rule]._production[item->_dot]));

        /* se o goto aplicado no elemento resultou num novo conjunto, adiciona */
        if (j.size()) {

          //cout << endl << "goto({";
          //printItemList(*s);
          //cout << "}, " << _v[item->_rule]._production[item->_dot].toString() << " ) -> ";
          //printItemList(j);

          ItemList *dst(new ItemList(j));

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

void testFirst(Grammar &g, const Symbol &s)
{
  SymbolSet a(g.first(s));

  for (auto i(a.begin()); i != a.end(); i++) {
    cout << i->toString() << ", ";
  }
  cout << endl;
}

void testFirst(Grammar &g, const SymbolList &s)
{
  SymbolSet a(g.first(s));

  for (auto i(a.begin()); i != a.end(); i++) {
    cout << i->toString() << ", ";
  }
  cout << endl;
}

Grammar gt({
  {F,{{D}},1},
  {F,{{K}},1},
  {D,{{"a"},{B}},1},
  {D,{{Symbol()}},1},
  {K,{{"c"},{H}},1},
  {H,{{"h"}},1}
});

Grammar g ({
  {EL,{{E}},1},
  {E,{{E},{"+"},{T}},1},
  {E,{{T}},1},
  {T,{{T},{"*"},{F}},1},
  {T,{{F}},1},
  {F,{{"("},{E},{")"}},1},
  {F,{{"id"}},1}
});

Grammar gE({
  {EL,{{""}},1},
  {EL,{{"oie"}},1},
  {EL,{{E}},1},
  {E,{{E}},1},
  {E,{{"lal"}},1},
  {E,{{},{"depois do vazio"}},1}
});

/*Grammar jsonG({
  {OBJECT,{{"{}"}},1},
  {OBJECT,{{MEMBERS}},1},
  
});*/

void testClosure()
{
  ItemList s(g.closure({{0,0,{}}}));
 
  cout << "closure final" << endl; 
  g.printItemList(s);
}

int main(int argc, char **argv)
{
  //CanonicalPair c(g.canonical({{0,0,{}}}));

  //cout << "nº de closures: " << c.first.size() << " e de itens: " << c.second.size() << endl;

  //testFirst(gE, EL);

  //testFirst(gE, {{EL},{"aia"}});

  //testSymbol();

  testClosure();

  return 0;
}
