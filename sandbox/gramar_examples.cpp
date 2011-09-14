#include "mygrammar.h"

MyGrammar g(symbolToString,{
  {EL,{{E}},1},
  {E,{{E},{PLUS},{T}},1},
  {E,{{T}},1},
  {T,{{T},{TIMES},{F}},1},
  {T,{{F}},1},
  {F,{{LPAR},{E},{RPAR}},1},
  {F,{{ID}},1}
},TEOF,INVALID);

MyGrammar ifG(symbolToString,{
  {EL,{{E}},1},
  {E,{{IF},{LPAR},{ID},{RPAR},{THEN},{V}},1},
  {E,{{IF},{LPAR},{ID},{RPAR},{THEN},{V},{ELSE},{V}},1},
  {V,{{E}},1},
  {V,{{ID}},1},
  {V,{},1}
},TEOF,INVALID);

MyGrammar mCICp66(symbolToString,{
  {SL,{{S},{TEOF}},1},
  {S,{{V},{ATTR},{E}},1},
  {S,{{E}},1},
  {E,{{V}},1},
  {V,{{ID}},1},
  {V,{{TIMES},{E}},1}
},TEOF,INVALID);

/* uma gramática com um monte de parenteses abrindo e fechando */
MyGrammar parG(symbolToString,{
  {EL,{{V},{TEOF}},1},
  {V,{{E}},1},
  {E,{{LPAR},{F},{RPAR},{ID}},1},
  {F,{{E},{IF}},1},
  {F,{},1}
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
  MyGrammar::LR1Table table(g.createTable());

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

  MyAutomata automata(parG);

  return 0;
}
