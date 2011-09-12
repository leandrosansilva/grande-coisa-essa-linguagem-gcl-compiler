#ifndef GRAMMAR_TABLE_H
#define GRAMMAR_TABLE_H

#include <map>

#include <tuple>

#include <iostream>

using namespace std;

typedef enum {
  GOTO, ACCEPT, ERROR, SHIFT, REDUCE
} Action;

struct TableAction
{
  Action _action;

  int _state;

  /* para estados accept e error */
  TableAction(Action action): _action(action)
  {
  }

  /* para shift, goto e reduce */
  TableAction(Action action, int state): _action(action), _state(state)
  {
  }

  /* só para satisfazer o compilador */
  TableAction(): _action(ERROR), _state(0)
  {
  }
};

template<typename SymbolT>
struct Table: public map<tuple<int,SymbolT>,TableAction>
{
};

#endif
