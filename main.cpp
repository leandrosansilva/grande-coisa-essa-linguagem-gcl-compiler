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
  {EL,"E'"},
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


MyGrammar g(symbolToString,{
  {EL,{{E}},1},
  {E,{{E},{"+"},{T}},1},
  {E,{{T}},1},
  {T,{{T},{"*"},{F}},1},
  {T,{{F}},1},
  {F,{{"("},{E},{")"}},1},
  {F,{{"id"}},1}
});

MyGrammar gt(symbolToString,{
  {F,{{D}},1},
  {F,{{K}},1},
  {D,{{"a"},{B}},1},
  {D,{},1},
  {K,{{"c"},{H}},1},
  {H,{{"h"}},1}
});

MyGrammar gE(symbolToString,{
  {EL,{{E}},1},
  {E,{{E}},1},
  {E,{{"lal"}},1},
  {E,{{},{"depois do vazio"}},1}
});

MyGrammar mCICp66(symbolToString,{
  {SL,{{S}},1},
  {S,{{V},{"="},{E}},1},
  {S,{{E}},1},
  {E,{{V}},1},
  {V,{{"x"}},1},
  {V,{{"*"},{E}},1}
});

MyGrammar ifG(symbolToString,{
  {EL,{{E}},1},
  {E,{{"if"},{"("},{V},{")"},{F},{"else"},{T}},1},
  {E,{{"if"},{"("},{V},{")"},{F}},1},
  {V,{{"condition"}},1},
  {F,{{"if_block"}},1},
  {F,{{E}},1},
  {T,{{"else_block"}},1},
  {T,{{E}},1}
});

MyGrammar simpleIfG(symbolToString,{
  {EL,{{E}},1},
  {E,{{"if"},{"("},{V},{")"},{E},{"else"},{E}},1},
  {V,{{"true"}},1},
  {V,{{"false"}},1},
  {E,{{"block"}},1}
});

MyGrammar simpleG(symbolToString,{
  {EL,{{E}},1},
  {E,{{"("},{F},{")"}},1},
  {F,{{"option"}},1},
  {F,{{E}},1},
});

int main(int argc, char **argv)
{
  simpleG.generateGraph();

  return 0;
}
