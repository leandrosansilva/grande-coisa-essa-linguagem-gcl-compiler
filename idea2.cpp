#include <vector>
#include <string>
#include <map>
#include <iostream>

using namespace std;

typedef enum
{
  IFSTM, THENSTM, ELSESTM, CONDITION
} NonTerminal;

struct Symbol
{
  typedef enum { TERMINAL, NONTERMINAL} Type;

  Type _type;
  const string _lexema;

  NonTerminal _nT;

  Symbol(const string &tok): _type(TERMINAL),_lexema(tok)
  {
  }
  
  Symbol(const NonTerminal &nT): _type(NONTERMINAL),_nT(nT)
  {
  }
};

typedef vector<Symbol> SymbolList;

struct Action
{
  SymbolList _sL;
  int _semAct;
  Action(const SymbolList &symbolList, int number):_sL(symbolList), _semAct(number)
  {
  };
};

typedef vector<Action> ActionList;

struct Rule
{
  ActionList _aL;
  bool _nullable;
  Rule(const ActionList &v): _aL(v)
  {
    ActionList::const_iterator it(_aL.begin());
    while (it != _aL.end() && it->_sL.size())
    {
      it++;
    }
    
    _nullable = it == _aL.end() ? false : true;
  };
  
  bool nullable() const
  {
    return _nullable;
  }
};

class Grammar
{
  map<NonTerminal,Rule> _map;
public:
  Grammar(const map<NonTerminal,Rule> &map): _map(map)
  {
  }
  
  Rule &rule(const NonTerminal &t)
  {
    return _map.at(t);
  }
};

Grammar g ({
  {IFSTM,
    Rule({
      {
        {{"if"},{"("} ,{CONDITION},{")"},{"then"},{THENSTM},{"else"},{ELSESTM}},1
      },

      {
        {{"if"},{"("} ,{CONDITION},{")"},{"then"},{THENSTM}},1
      }
    })
  },

  {CONDITION,Rule({{{{"a"},{">"},{"b"}},2}})},

  {THENSTM,Rule({{{{"true"}},3}})},
           
  {ELSESTM,Rule({{{{"false"}},3},{{},4}})}
});

int main(int argc, char **argv)
{
  cout << (g.rule(ELSESTM).nullable() ? "TRUE" : "FALSE") << endl;
  cout << (g.rule(IFSTM).nullable() ? "TRUE" : "FALSE") << endl;
  cout << (g.rule(THENSTM).nullable() ? "TRUE" : "FALSE") << endl;
  cout << (g.rule(CONDITION).nullable() ? "TRUE" : "FALSE") << endl;

  return 0;
}
