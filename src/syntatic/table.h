#ifndef GRAMMAR_TABLE_H
#define GRAMMAR_TABLE_H

#include <map>

#include <tuple>

#include <iostream>

#include <string>

#include <sstream>

#include <fstream>

using namespace std;

namespace Syntatical {

typedef enum {
  GOTO, ACCEPT, ERROR, SHIFT, REDUCE
} Action;

static map<Action,string> ActionToString {
  {GOTO,"G"},
  {ACCEPT,"A"},
  {ERROR,"E"},
  {SHIFT,"S"},
  {REDUCE,"R"}
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
  string actionToString(const Action &action) const
  {
    return ActionToString[action];
  }
};

}

// operador de serialização
template<typename SymbolT>
void operator>>(const Syntatical::Table<SymbolT> &table,fstream &file)
{
  for (auto action(table.begin()); action != table.end(); action++) {
    file << get<0>(action->first);
    file << " ";
    if (get<1>(action->first).isTerminal()) {
      file << "T ";
      file << int(get<1>(action->first)._terminal);
    } else if (get<1>(action->first).isNonTerminal()) { 
      file << "N ";
      file << int(get<1>(action->first)._nonTerminal);
    } else {
      file << "E";
    }
    file << " ";
    file << table.actionToString(action->second._action);
    file << " ";
    if (action->second._action != Syntatical::ACCEPT) {
      file << action->second._value; 
    } 
    file << "\n";
  }
}

// operador de deserialização
template<typename SymbolT>
void operator<<(Syntatical::Table<SymbolT> &table, fstream &file)
{
  // lê todo o conteúdo do arquivo para uma string
  int size(file.tellp());
  char *buffer(new char[size]);
  file.seekg(0,ios::beg);
  file.read(reinterpret_cast<char *>(buffer),size);
}

#endif
