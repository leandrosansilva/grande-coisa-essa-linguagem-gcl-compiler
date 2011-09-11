#include <mygrammar.h>

MyGrammar g(symbolToString,{
  {EL,{{E}},1},
  {E,{{E},{PLUS},{T}},1},
  {E,{{T}},1},
  {T,{{T},{TIMES},{F}},1},
  {T,{{F}},1},
  {F,{{LPAR},{E},{RPAR}},1},
  {F,{{ID}},1}
},TEOF,INVALID);

MyGrammar mCICp66(symbolToString,{
  {SL,{{S},{TEOF}},1},
  {S,{{V},{ATTR},{E}},1},
  {S,{{E}},1},
  {E,{{V}},1},
  {V,{{ID}},1},
  {V,{{TIMES},{E}},1}
},TEOF,INVALID);

void testCanonical(MyGrammar &g)
{
  MyGrammar::CanonicalItems p(g.items());

  for (auto e(p.second.begin()); e != p.second.end(); e++) {
    cout << e->first.first << " to " << e->first.second << " s " 
         << g._symbolToString(e->second) << endl;
  }

  for (auto s(p.first.begin()); s!= p.first.end(); s++) {
    cout << "set " << distance(p.first.begin(),s) << endl;

    for (auto i(s->begin()); i != s->end(); i++) {
      cout << g.itemToString(*i) << endl;
    }
  }
}

int main(int argc, char **argv)
{
  mCICp66.generateGraph();

  //testCanonical(mCICp66);

  return 0;
}
