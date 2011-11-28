#include <experiments/mygrammar.h>

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

  MyGrammar::Symbol s3(ID);
  MyGrammar::Symbol s4(TIMES);

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

  MyGrammar::SymbolList list { {ID}, {E}, {}, {PLUS}, {EL}, {E}, {ATTR}, {}, {IF}, {T}, {}, {}};

  std::sort(list.begin(), list.end());
    
  for (int i(0); i< list.size(); i++) {
    cout << symbolToString(list[i]) << ", ";
  }
  cout << endl;
}

void testItem()
{
  MyGrammar::Item i1(2,2,MyGrammar::Symbol(IF));
  MyGrammar::Item i2(2,2,MyGrammar::Symbol(THEN));
  MyGrammar::Item i3(2,2,MyGrammar::Symbol(E));
  MyGrammar::Item i4(2,3,MyGrammar::Symbol(ELSE));
  MyGrammar::Item i5(3,2,MyGrammar::Symbol(EQUAL));
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

  cout << "goto " << symbolToString({V}) << endl;

  g.printItemList(g.goTo(s,{V}));

  cout << g.goTo(s,{V}).size() << endl;

  cout << endl;
}

void testItemSet()
{
  set<MyGrammar::Item> a;

  a.insert({1,2,{IF}});
  a.insert({2,3,{THEN}});
  a.insert({2,3,{ELSE}});
  a.insert({1,2,{ID}});

  for (auto i(a.begin()); i != a.end(); i++) {
    cout << i->rule() << ", " << i->dot() << ", " << symbolToString(i->symbol()) << endl;
  }

  TEST(a.find({2,3,{ELSE}}) == a.end(),"FALSE");
  TEST(a.find({2,3,{THEN}}) != a.end(),"TRUE");
  TEST(a.find({3,2,{RPAR}}) == a.end(),"TRUE");
  TEST(a.find({2,3,{ELSE}}) == a.end(),"FALSE");
  TEST(a.find({1,2,{ID}}) == a.end(),"FALSE");
  TEST(a.find({1,3,{V}}) == a.end(),"TRUE");
  TEST(MyGrammar::Item(2,3,{ID}) == MyGrammar::Item(2,3,{ID}),"TRUE");
  TEST(MyGrammar::Item(2,3,{IF}) > MyGrammar::Item(3,3,{ID}),"FALSE");
  TEST(MyGrammar::Item(2,3,{ATTR}) > MyGrammar::Item(2,3,{PLUS}),"TRUE");
  TEST(MyGrammar::Item(1,2,{IF}) == MyGrammar::Item(1,2,{ELSE}),"FALSE");
}

void testCanonical(MyGrammar &g)
{
  MyGrammar::CanonicalItems c(g.items());

  cout << "nº de closures: " << get<0>(c).size() << " e de itens: " << get<1>(c).size() << endl;
}

MyGrammar g(symbolToString,{
  {EL,{{E}}},
  {E,{{E},{PLUS},{T}}},
  {E,{{T}}},
  {T,{{T},{TIMES},{F}}},
  {T,{{F}}},
  {F,{{LPAR},{E},{RPAR}}},
  {F,{{ID}}}
},TEOF,INVALID);

MyGrammar mCICp66(symbolToString,{
  {SL,{{S},{TEOF}}},
  {S,{{V},{ATTR},{E}}},
  {S,{{E}}},
  {E,{{V}}},
  {V,{{ID}}},
  {V,{{PLUS},{E}}}
},TEOF,INVALID);

void testItem2()
{
  MyGrammar::Item item(3,2,{S});

  cout << g.itemToString(item) << endl;
}

int main(int argc, char **argv)
{
  testItem2();
  testItemSet();
  testSymbol();
}
