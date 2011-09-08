#include <grammar.h>

typedef enum
{
  EL,E, T, F,
  
  D,K,B,H,
  
  SL,S,V
} NonTerminal;

typedef enum
{
  IF,THEN,ELSE,LPAR,RPAR,ID
} Terminal;

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


typedef Grammar<NonTerminal,Terminal> MyGrammar;

string symbolToString(const MyGrammar::Symbol &s)
{
  if (s.isTerminal()) {
    return s._lexema;
  }
  if (s.isNonTerminal()) {
    return NonTerminalMap[s._nT];
  }

  if (s.isEmpty()) {
    return "$";
  }
}


bool TEST(const bool ret, const string &msg)
{
  cout << msg << ": " << (ret ? "TRUE" : "FALSE") << endl;
}

void testSymbol()
{
  MyGrammar::Symbol s1(E);
  MyGrammar::Symbol s2(T);

  /* dois vazios */
  MyGrammar::Symbol v1, v2;

  MyGrammar::Symbol s3("primeiro");
  MyGrammar::Symbol s4("segundo");

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

  MyGrammar::SymbolList list { {"a"}, {E}, {}, {"b"}, {EL}, {E}, {"zaza"}, {}, {"a"}, {T}, {}, {}};

  std::sort(list.begin(), list.end());
    
  for (int i(0); i< list.size(); i++) {
    cout << symbolToString(list[i]) << ", ";
  }
  cout << endl;
}

void testItem()
{
  MyGrammar::Item i1(2,2,MyGrammar::Symbol("a"));
  MyGrammar::Item i2(2,2,MyGrammar::Symbol("b"));
  MyGrammar::Item i3(2,2,MyGrammar::Symbol(E));
  MyGrammar::Item i4(2,3,MyGrammar::Symbol("b"));
  MyGrammar::Item i5(3,2,MyGrammar::Symbol("b"));
  MyGrammar::Item i6(2,2,MyGrammar::Symbol(EL));

  TEST(i1 < i2,"TRUE");
  TEST(i1 != i1,"FALSE");
  TEST(i1 < i3,"TRUE");
  TEST(i3 > i1,"TRUE");
  TEST(i3 == i1,"FALSE");
  TEST(i6 == i3,"FALSE");
  TEST(i6 < i3,"TRUE");
}

void testFirst(MyGrammar &g, const MyGrammar::Symbol &s)
{
  MyGrammar::SymbolSet a(g.first(s));

  for (auto i(a.begin()); i != a.end(); i++) {
    cout << symbolToString(*i) << ", ";
  }
  cout << endl;
}

void testFirst(MyGrammar &g, const MyGrammar::SymbolList &s)
{
  MyGrammar::SymbolSet a(g.first(s));

  for (auto i(a.begin()); i != a.end(); i++) {
    cout << symbolToString(*i) << ", ";
  }
  cout << endl;
}

void testClosure(MyGrammar &g)
{
  MyGrammar::ItemList s(g.closure({{0,0,{}}}));
 
  cout << "closure final" << endl; 
  g.printItemList(s);

  cout << endl;

  cout << "goto " << symbolToString(MyGrammar::Symbol("if")) << endl;

  g.printItemList(g.goTo(s,{"else"}));

  cout << g.goTo(s,{"else"}).size() << endl;

  cout << endl;
  
}

void testItemSet()
{
  set<MyGrammar::Item> a;

  a.insert({1,2,{"C"}});
  a.insert({2,3,{"B"}});
  a.insert({2,3,{"C"}});
  a.insert({1,2,{"B"}});

  for (auto i(a.begin()); i != a.end(); i++) {
    cout << i->_rule << ", " << i->_dot << ", " << symbolToString(i->_s) << endl;
  }

  TEST(a.find({2,3,{"C"}}) == a.end(),"FALSE");
  TEST(a.find({2,3,{"B"}}) != a.end(),"TRUE");
  TEST(a.find({3,2,{"B"}}) == a.end(),"TRUE");
  TEST(a.find({1,2,{"B"}}) == a.end(),"FALSE");
  TEST(a.find({1,2,{"C"}}) == a.end(),"FALSE");
  TEST(a.find({1,3,{"C"}}) == a.end(),"TRUE");
  TEST(MyGrammar::Item(2,3,{"C"}) == MyGrammar::Item(2,3,{"C"}),"TRUE");
  TEST(MyGrammar::Item(2,3,{"B"}) > MyGrammar::Item(3,3,{"C"}),"FALSE");
  TEST(MyGrammar::Item(2,3,{"C"}) > MyGrammar::Item(2,3,{"B"}),"TRUE");
  TEST(MyGrammar::Item(1,2,{"B"}) == MyGrammar::Item(1,2,{"C"}),"FALSE");
  TEST(MyGrammar::Item(1,2,{"C"}) == MyGrammar::Item(1,2,{"B"}),"FALSE");
  TEST(MyGrammar::Item(2,3,{"B"}) > MyGrammar::Item(1,2,{"C"}),"FALSE");
  TEST(MyGrammar::Item(2,3,{"B"}) < MyGrammar::Item(1,2,{"C"}),"TRUE");
}

void testCanonical(MyGrammar &g)
{
  MyGrammar::CanonicalPair c(g.items());

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
}

int main(int argc, char **argv)
{
  MyGrammar g(symbolToString,{
    {EL,{{E}},1},
    {E,{{E},{"+"},{T}},1},
    {E,{{T}},1},
    {T,{{T},{"*"},{F}},1},
    {T,{{F}},1},
    {F,{{"("},{E},{")"}},1},
    {F,{{"id"}},1}
  });

  testSymbol();

}
