#ifndef GRAMMAR_TABLE_H
#define GRAMMAR_TABLE_H

#include <map>

#include <tuple>

#include <iostream>

#include <string>

using namespace std;

namespace Syntatical {

typedef enum {
  GOTO, ACCEPT, ERROR, SHIFT, REDUCE
} Action;

static map<Action,string> ActionToString {
  {GOTO,"GOTO"},
  {ACCEPT,"ACCEPT"},
  {ERROR,"ERROR"},
  {SHIFT,"SHIFT"},
  {REDUCE,"REDUCE"}
};

struct TableAction
{
  Action _action;

  int _value;

  /* para estados accept e error */
  TableAction(Action action): _action(action)
  {
  }

  /* para shift, goto e reduce */
  TableAction(Action action, int value): _action(action), _value(value)
  {
  }

  /* só para satisfazer o compilador */
  TableAction(): _action(ERROR), _value(0)
  {
  }
};

template<typename SymbolT>
struct Table: public map<tuple<int,SymbolT>,TableAction>
{
  string actionToString(const Action action)
  {
    return ActionToString[action];
  }
};

}

#endif
