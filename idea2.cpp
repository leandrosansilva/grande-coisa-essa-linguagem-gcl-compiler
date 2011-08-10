#include <vector>
#include <string>

using namespace std;

typedef enum
{
  IFSTM, THENSTM, ELSESTM, CONDITION
} NonTerminal;

struct Symbol
{
  Symbol(const string &tok)
  {
  }
  
  Symbol(const NonTerminal &nT)
  {
  }
};

typedef vector<Symbol> SymbolList;

struct Action
{
  Action(const SymbolList &symbolList, int number)
  {
  };
};

typedef vector<Action> ActionList;

struct DerivationList
{
  DerivationList(const ActionList &v)
  {
  };
};

struct Rule
{
  Rule(const NonTerminal &t,const DerivationList &d) 
  {
  };
};

typedef vector<Rule> RuleList;

struct Grammar
{
  Grammar(const RuleList &list)
  {};
};

DerivationList d1 ({
  {
    {{"if"},{"("} ,{CONDITION},{")"},{"then"},{THENSTM},{"else"},{ELSESTM}},1
  },

  {
    {{"if"},{"("} ,{CONDITION},{")"},{"then"},{THENSTM},{"else"},{ELSESTM}},1
  }
});

Rule rule(IFSTM,
  DerivationList({
    {
      {{"if"},{"("} ,{CONDITION},{")"},{"then"},{THENSTM},{"else"},{ELSESTM}},1
    },

    {
      {{"if"},{"("} ,{CONDITION},{")"},{"then"},{THENSTM},{"else"},{ELSESTM}},1
    }
  })
);

Grammar g ({
  Rule(IFSTM,
    DerivationList({
      {
        {{"if"},{"("} ,{CONDITION},{")"},{"then"},{THENSTM},{"else"},{ELSESTM}},1
      },

      {
        {{"if"},{"("} ,{CONDITION},{")"},{"then"},{THENSTM}},1
      }
    })
  ),
  Rule(CONDITION,
    DerivationList({
      {
        {{"a"},{">"},{"b"}},2
      }
    })
  ),
  Rule(THENSTM,
    DerivationList({
      {
        {{"true"}},3
      }
    })
  ),
  Rule(ELSESTM,
    DerivationList({
      {
        {{"false"}},3
      }
    })
  )
});

int main(int argc, char **argv)
{
  return 0;
}
