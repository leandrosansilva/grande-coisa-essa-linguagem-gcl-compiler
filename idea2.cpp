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
  
  D,K,B,H,
  
  SL,S,V
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
  {H,"H"},
  {SL,"SL"},
  {S,"S"},
  {V,"V"}
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
    return !(*this == other || *this > other);
  }

  bool operator>(const Item &other) const
  {
    if (_s > other._s) {
      return true;
    }

    if (_s < other._s) {
      return false;
    }

    if ((_rule * 1000 + _dot) > (other._rule * 1000 + other._dot)) {
      return true;
    }
    
    return false;
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
    cout << "< ";
    
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

    cout << ">, #la: '" << item._s.toString() << "'; " << endl;

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

      /* não aplico às regras onde o ponto está no final! */
      if (curItem._dot >= _v[curItem._rule]._production.size()) {
        continue;
      }

      /* se o ponto do item não se encntra num não-terminal, nada a ser feito */
      if (!_v[curItem._rule]._production[curItem._dot].isNonTerminal()) {
        continue;
      }

      /* concateno todos os símbolos depois do ponto com o lookahead */
      SymbolList withLA(
        _v[curItem._rule]._production.begin() + curItem._dot + 1,
        _v[curItem._rule]._production.end()
      );

      /* concateno o lookahead */
      withLA.push_back(curItem._s);

      /* O first do conjunto acima diz quais símbolos vêm após ele  */
      SymbolSet f(first(withLA));

      /* limpo o cara que guarda as informações do first: FIXME: side-effect */
      _firstChecked.clear();

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

  CanonicalPair items()
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

      /* para cada item no conjunto */
      for (auto item(s->begin()); item != s->end(); item++) {


        // Se o item já foi usado, vou ao próximo
        if (usedItems.find(*item) != usedItems.end()) {
          continue;
        }

        // para cada cara depois do ponto da regra em questão, faz o goto dele
        ItemList j(goTo(*s,_v[item->_rule]._production[item->_dot]));

        /* se o goto aplicado no elemento não resultou num novo conjunto, saio */
        if (!j.size()) {
          continue;
        }

        ItemList *dst(new ItemList(j));

        t.push_back(dst);

        usedItems[*item] = dst;
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
  {D,{},1},
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

Grammar mCICp66({
  {SL,{{S}},1},
  {S,{{V},{"="},{E}},1},
  {S,{{E}},1},
  {E,{{V}},1},
  {V,{{"x"}},1},
  {V,{{"*"},{E}},1}
});

Grammar ifG({
  {EL,{{E}},1},
  {E,{{"if"},{"("},{V},{")"},{F},{"else"},{T}},1},
  {E,{{"if"},{"("},{V},{")"},{F}},1},
  {V,{{"condition"}},1},
  {F,{{"if_block"}},1},
  {F,{{E}},1},
  {T,{{"else_block"}},1},
  {T,{{E}},1}
});

Grammar simpleG({
  {EL,{{E}},1},
  {E,{{"ai"},{E},{"ia"}},1},
  {E,{{"ui"}},1}
});

/*Grammar jsonG({
  {OBJECT,{{"{}"}},1},
  {OBJECT,{{MEMBERS}},1},
});*/

void testClosure()
{
  ItemList s(ifG.closure({{0,0,{}}}));
 
  cout << "closure final" << endl; 
  ifG.printItemList(s);

  cout << endl;

  cout << "goto " << Symbol("if").toString() << endl;

  ifG.printItemList(ifG.goTo(s,{"else"}));

  cout << ifG.goTo(s,{"else"}).size() << endl;

  cout << endl;
  
}

void testItemSet()
{
  set<Item> a;

  a.insert({1,2,{"C"}});
  a.insert({2,3,{"B"}});
  //a.insert({2,3,{"C"}});
  //a.insert({1,2,{"B"}});

  for (auto i(a.begin()); i != a.end(); i++) {
    cout << i->_rule << ", " << i->_dot << ", " << i->_s.toString() << endl;
  }

  //TEST(a.find({2,3,{"C"}}) == a.end(),"FALSE");
  TEST(a.find({2,3,{"B"}}) != a.end(),"TRUE");
  TEST(a.find({3,2,{"B"}}) == a.end(),"TRUE");
  //TEST(a.find({1,2,{"B"}}) == a.end(),"FALSE");
  //TEST(a.find({1,2,{"C"}}) == a.end(),"FALSE");
  //TEST(a.find({1,3,{"C"}}) == a.end(),"TRUE");
  //TEST(Item(2,3,{"C"}) == Item(2,3,{"C"}),"TRUE");
  //TEST(Item(2,3,{"B"}) > Item(3,3,{"C"}),"FALSE");
  //TEST(Item(2,3,{"C"}) > Item(2,3,{"B"}),"TRUE");
  TEST(Item(1,2,{"B"}) == Item(1,2,{"C"}),"FALSE");
  TEST(Item(1,2,{"C"}) == Item(1,2,{"B"}),"FALSE");
  TEST(Item(2,3,{"B"}) > Item(1,2,{"C"}),"FALSE");
  TEST(Item(2,3,{"B"}) < Item(1,2,{"C"}),"TRUE");
}

void testCanonical(Grammar &g)
{
  CanonicalPair c(g.items());

  cout << "nº de closures: " << c.first.size() << " e de itens: " << c.second.size() << endl;

  for (auto s(c.first.begin()); s != c.first.end(); s++) {
    cout << "new set " << dec << (unsigned long long int)*s << endl;
    for (auto i((*s)->begin()); i != (*s)->end(); i++) {
      g.printItem(*i);
    }
  }

  cout << "gotos, etc" << endl;
  
  for (auto i(c.second.begin()); i != c.second.end(); i++) {
    cout << dec << (unsigned long long)i->second << endl;
    g.printItem(i->first);
    cout << endl;
  }

  cout << "checagem" << endl;

  for (auto s(c.first.begin()); s != c.first.end(); s++) {
    cout << "new set " << dec << (unsigned long long int)*s << endl;
    for (auto i((*s)->begin()); i != (*s)->end(); i++) {
      if (g._v[i->_rule]._production.size() == i->_dot) {
        break;
      }

      if (c.second.find(*i) == c.second.end()) {
        cout << "não achou" << endl;
        g.printItem(*i);
      }
    }
  }
 

}

void testItemMap()
{
  vector<Item> v {
    {1,2,{}},
    {1,2,{"+"}},
    {1,2,{}},
    {1,2,{"+"}},
    {3,2,{}},
    {3,2,{"+"}},
    {3,2,{"*"}},
    {3,2,{}},
    {3,2,{"+"}},
    {3,2,{"*"}},
    {3,2,{}},
    {3,2,{"+"}},
    {3,2,{"*"}},
    {5,2,{"+"}},
    {5,2,{"*"}},
    {5,2,{"+"}},
    {5,2,{"*"}},
    {5,2,{"+"}},
    {5,2,{"*"}},
    {5,2,{"+"}},
    {5,2,{"*"}},
    {5,2,{")"}},
    {5,2,{")"}},
    {5,2,{")"}}
  };

  map<Item,char *> m {
    {{0,0,{}},NULL},
    {{1,0,{}},NULL},
    {{2,0,{}},NULL},
    {{1,0,{"+"}},NULL},
    {{2,0,{"+"}},NULL},
    {{3,0,{}},NULL},
    {{4,0,{}},NULL},
    {{3,0,{"+"}},NULL},
    {{4,0,{"+"}},NULL},
    {{3,0,{"*"}},NULL},
    {{4,0,{"*"}},NULL},
    {{5,0,{}},NULL},
    {{6,0,{}},NULL},
    {{5,0,{"+"}},NULL},
    {{6,0,{"+"}},NULL},
    {{5,0,{"*"}},NULL},
    {{6,0,{"*"}},NULL},
    {{0,1,{}},NULL},
    {{1,1,{}},NULL},
    {{1,1,{"+"}},NULL},
    {{0,1,{}},NULL},
    {{2,1,{}},NULL},
    {{2,1,{"+"}},NULL},
    {{3,1,{}},NULL},
    {{3,1,{"+"}},NULL},
    {{3,1,{"*"}},NULL},
    {{0,1,{}},NULL},
    {{2,1,{}},NULL},
    {{2,1,{"+"}},NULL},
    {{2,1,{}},NULL},
    {{2,1,{"+"}},NULL},
    {{4,1,{}},NULL},
    {{4,1,{"+"}},NULL},
    {{4,1,{"*"}},NULL},
    {{2,1,{}},NULL},
    {{2,1,{"+"}},NULL},
    {{4,1,{}},NULL},
    {{4,1,{"+"}},NULL},
    {{4,1,{"*"}},NULL},
    {{2,1,{}},NULL},
    {{2,1,{"+"}},NULL},
    {{4,1,{}},NULL},
    {{4,1,{"+"}},NULL},
    {{4,1,{"*"}},NULL},
    {{5,1,{}},NULL},
    {{5,1,{"+"}},NULL},
    {{5,1,{"*"}},NULL},
    {{1,0,{")"}},NULL},
    {{2,0,{")"}},NULL},
    {{3,0,{")"}},NULL},
    {{4,0,{")"}},NULL},
    {{5,0,{")"}},NULL},
    {{6,0,{")"}},NULL},
    {{6,1,{}},NULL},
    {{6,1,{"+"}},NULL},
    {{6,1,{"*"}},NULL},
    {{6,1,{}},NULL},
    {{6,1,{"+"}},NULL},
    {{6,1,{"*"}},NULL},
    {{6,1,{}},NULL},
    {{6,1,{"+"}},NULL},
    {{6,1,{"*"}},NULL},
    {{1,2,{}},NULL},
    {{1,2,{"+"}},NULL},
    {{1,2,{}},NULL},
    {{1,2,{"+"}},NULL},
    {{3,2,{}},NULL},
    {{3,2,{"+"}},NULL},
    {{3,2,{"*"}},NULL},
    {{3,2,{}},NULL},
    {{3,2,{"+"}},NULL},
    {{3,2,{"*"}},NULL},
    {{3,2,{}},NULL},
    {{3,2,{"+"}},NULL},
    {{3,2,{"*"}},NULL},
    {{5,2,{}},NULL},
    {{5,2,{"+"}},NULL},
    {{5,2,{"*"}},NULL},
    {{1,1,{")"}},NULL},
    {{1,1,{"+"}},NULL},
    {{5,2,{"+"}},NULL},
    {{5,2,{"*"}},NULL},
    {{5,2,{"+"}},NULL},
    {{5,2,{"*"}},NULL},
    {{5,2,{"+"}},NULL},
    {{5,2,{"*"}},NULL},
    {{2,1,{")"}},NULL},
    {{2,1,{"+"}},NULL},
    {{3,1,{")"}},NULL},
    {{3,1,{"+"}},NULL},
    {{3,1,{"*"}},NULL},
    {{2,1,{")"}},NULL},
    {{2,1,{"+"}},NULL},
    {{4,1,{")"}},NULL},
    {{4,1,{"+"}},NULL},
    {{4,1,{"*"}},NULL},
    {{5,1,{")"}},NULL},
    {{5,1,{"+"}},NULL},
    {{5,1,{"*"}},NULL},
    {{6,1,{")"}},NULL},
    {{6,1,{"+"}},NULL},
    {{6,1,{"*"}},NULL},
    {{1,3,{}},NULL},
    {{1,3,{"+"}},NULL},
    {{3,1,{}},NULL},
    {{1,3,{}},NULL},
    {{1,3,{"+"}},NULL},
    {{1,3,{}},NULL},
    {{1,3,{"+"}},NULL},
    {{1,3,{}},NULL},
    {{1,3,{"+"}},NULL},
    {{3,3,{}},NULL},
    {{3,3,{"+"}},NULL},
    {{3,3,{"*"}},NULL},
    {{3,3,{}},NULL},
    {{3,3,{"+"}},NULL},
    {{3,3,{"*"}},NULL},
    {{3,3,{}},NULL},
    {{3,3,{"+"}},NULL},
    {{3,3,{"*"}},NULL},
    {{3,3,{}},NULL},
    {{3,3,{"+"}},NULL},
    {{3,3,{"*"}},NULL},
    {{3,3,{}},NULL},
    {{3,3,{"+"}},NULL},
    {{3,3,{"*"}},NULL},
    {{3,3,{}},NULL},
    {{3,3,{"+"}},NULL},
    {{3,3,{"*"}},NULL},
    {{3,3,{}},NULL},
    {{3,3,{"+"}},NULL},
    {{3,3,{"*"}},NULL},
    {{3,3,{}},NULL},
    {{3,3,{"+"}},NULL},
    {{3,3,{"*"}},NULL},
    {{3,3,{}},NULL},
    {{3,3,{"+"}},NULL},
    {{3,3,{"*"}},NULL},
    {{5,3,{}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{5,3,{}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{5,3,{}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{1,2,{")"}},NULL},
    {{5,3,{}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{5,3,{}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{5,3,{}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{5,3,{}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{5,3,{}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{5,3,{}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{3,2,{")"}},NULL},
    {{5,2,{")"}},NULL},
    {{5,2,{")"}},NULL},
    {{5,2,{")"}},NULL},
    {{1,3,{")"}},NULL},
    {{1,3,{"+"}},NULL},
    {{3,3,{")"}},NULL},
    {{3,3,{"+"}},NULL},
    {{3,3,{"*"}},NULL},
    {{5,3,{")"}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{5,3,{")"}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},
    {{5,3,{")"}},NULL},
    {{5,3,{"+"}},NULL},
    {{5,3,{"*"}},NULL},

  };

  for (int i(0); i< v.size(); i++) {
    cout << (m.find(v[i]) == m.end() ? "FAIL" : "OK") << endl;
  }
}

int main(int argc, char **argv)
{
  //testFirst(gE, EL);

  //testFirst(gE, {{EL},{"aia"}});

  //testSymbol();

  //testClosure();

  //cout << endl;

  //testItemSet();

  testCanonical(g);

  //testItemMap();

  return 0;
}
