#include <grammar.h>

bool TEST(const bool ret, const string &msg)
{
  cout << msg << ": " << (ret ? "TRUE" : "FALSE") << endl;
}

void testSymbol()
{
  Grammar::Symbol s1(E);
  Grammar::Symbol s2(T);

  /* dois vazios */
  Grammar::Symbol v1, v2;

  Grammar::Symbol s3("primeiro");
  Grammar::Symbol s4("segundo");

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

  Grammar::SymbolList list { {"a"}, {E}, {}, {"b"}, {EL}, {E}, {"zaza"}, {}, {"a"}, {T}, {}, {}};

  std::sort(list.begin(), list.end());
    
  for (int i(0); i< list.size(); i++) {
    cout << list[i].toString() << ", ";
  }
  cout << endl;
}

void testItem()
{
  Grammar::Item i1(2,2,Grammar::Symbol("a"));
  Grammar::Item i2(2,2,Grammar::Symbol("b"));
  Grammar::Item i3(2,2,Grammar::Symbol(E));
  Grammar::Item i4(2,3,Grammar::Symbol("b"));
  Grammar::Item i5(3,2,Grammar::Symbol("b"));
  Grammar::Item i6(2,2,Grammar::Symbol(EL));

  TEST(i1 < i2,"TRUE");
  TEST(i1 != i1,"FALSE");
  TEST(i1 < i3,"TRUE");
  TEST(i3 > i1,"TRUE");
  TEST(i3 == i1,"FALSE");
  TEST(i6 == i3,"FALSE");
  TEST(i6 < i3,"TRUE");
}

void testFirst(Grammar &g, const Grammar::Symbol &s)
{
  Grammar::SymbolSet a(g.first(s));

  for (auto i(a.begin()); i != a.end(); i++) {
    cout << i->toString() << ", ";
  }
  cout << endl;
}

void testFirst(Grammar &g, const Grammar::SymbolList &s)
{
  Grammar::SymbolSet a(g.first(s));

  for (auto i(a.begin()); i != a.end(); i++) {
    cout << i->toString() << ", ";
  }
  cout << endl;
}

void testClosure(Grammar &g)
{
  Grammar::ItemList s(g.closure({{0,0,{}}}));
 
  cout << "closure final" << endl; 
  g.printItemList(s);

  cout << endl;

  cout << "goto " << Grammar::Symbol("if").toString() << endl;

  g.printItemList(g.goTo(s,{"else"}));

  cout << g.goTo(s,{"else"}).size() << endl;

  cout << endl;
  
}

void testItemSet()
{
  set<Grammar::Item> a;

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
  TEST(Grammar::Item(1,2,{"B"}) == Grammar::Item(1,2,{"C"}),"FALSE");
  TEST(Grammar::Item(1,2,{"C"}) == Grammar::Item(1,2,{"B"}),"FALSE");
  TEST(Grammar::Item(2,3,{"B"}) > Grammar::Item(1,2,{"C"}),"FALSE");
  TEST(Grammar::Item(2,3,{"B"}) < Grammar::Item(1,2,{"C"}),"TRUE");
}

void testCanonical(Grammar &g)
{
  Grammar::CanonicalPair c(g.items());

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
}
