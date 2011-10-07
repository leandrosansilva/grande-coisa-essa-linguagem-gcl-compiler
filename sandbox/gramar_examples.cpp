#include "mygrammar.h"

MyGrammar g(symbolToString,{
  {EL,{{E}}},
  {E,{{E},{PLUS},{T}}},
  {E,{{T}}},
  {T,{{T},{TIMES},{F}}},
  {T,{{F}}},
  {F,{{LPAR},{E},{RPAR}}},
  {F,{{ID}}}
},TEOF,INVALID);

MyGrammar ifG(symbolToString,{
  {EL,{{E}}},
  {E,{{IF},{LPAR},{ID},{RPAR},{THEN},{V}}},
  {E,{{IF},{LPAR},{ID},{RPAR},{THEN},{V},{ELSE},{V}}},
  {V,{{E}}},
  {V,{{ID}}},
  {V,{}}
},TEOF,INVALID);

MyGrammar mCICp66(symbolToString,{
  {SL,{{S},{TEOF}}},
  {S,{{V},{ATTR},{E}}},
  {S,{{E}}},
  {E,{{V}}},
  {V,{{ID}}},
  {V,{{TIMES},{E}}}
},TEOF,INVALID);

// uma gramática com um monte de parenteses abrindo e fechando
MyGrammar parG(symbolToString,{
  {EL,{{V},{TEOF}}},
  {V,{{E}}},
  {E,{{LPAR},{F},{RPAR},{ID}}},
  {F,{{E},{IF}}},
  {F,{}}
},TEOF,INVALID);

void testCanonical(MyGrammar &g)
{
  MyGrammar::CanonicalItems p(g.items());

  for (auto r(get<2>(p).begin()); r != get<2>(p).end(); r++) {
    cerr << "Item " << get<0>(*r) << ", la: " << g._symbolToString(get<1>(*r)) 
         << ", rule " << get<2>(*r) << endl;
  }

  for (auto e(get<1>(p).begin()); e != get<1>(p).end(); e++) {
    cerr << get<0>(*e).first << " to " << get<1>(*e) << " s "
         << g._symbolToString(get<0>(*e).second) << endl;
  }

  for (auto s(get<0>(p).begin()); s != get<0>(p).end(); s++) {
    cerr << "set " << distance(get<0>(p).begin(),s) << endl;

    for (auto i(s->begin()); i != s->end(); i++) {
      cerr << g.itemToString(*i) << endl;
    }
  }
}

void testTable(MyGrammar &g)
{
  MyGrammar::LR1Table table(g.createTable("test_grammar.txt"));

  for (auto r(table.begin()); r != table.end(); r++) {
    cerr << get<0>(r->first) << " " << g._symbolToString(get<1>(r->first))
         << " " << table.actionToString(r->second._action) << " " << r->second._value << endl;
  }
}

int main(int argc, char **argv)
{
  //testCanonical(mCICp66);

  //testTable(mCICp66);

  //mCICp66.generateGraph();

  //MySyntatical::Analyzer automata(parG);

  return 0;
}
