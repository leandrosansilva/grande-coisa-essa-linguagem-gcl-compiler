#ifndef GRAMMAR_TABLE_H
#define GRAMMAR_TABLE_H

#include <map>

#include <tuple>

#include <iostream>

#include <string>

#include <sstream>

#include <fstream>

#include <vector>

using namespace std;

// http://www.codeproject.com/KB/stl/Split_string.aspx
template <typename C>
size_t split(const string &s, C &container, const char delimiter, bool keepBlankFields = true )
{
  size_t n = 0;
  typename string::const_iterator it = s.begin(), end = s.end(), first;
  for (first = it; it != end; ++it)
  {
    // Examine each character and if it matches the delimiter
    if (delimiter == *it)
    {
      if (keepBlankFields || first != it)
      {
        // extract the current field from the string and
        // append the current field to the given container
        container.push_back(string(first, it));
        ++n;
        
        // skip the delimiter
        first = it + 1;
      }
      else
      {
        ++first;
      }
    }
  }
  if (keepBlankFields || first != it)
  {
    // extract the last field from the string and
    // append the last field to the given container
    container.push_back(string(first, it));
    ++n;
  }
  return n;
}

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

template<typename SymbolT>
tuple<int,SymbolT> createStateAndSymbol(const vector<string> &results)
{
  // preciso de um estado e de um símbolo

  // a primeira letra do primeiro que casou, diz qual a ação
  const char code(results[0][0]);
  
  int state;

  SymbolT symbol;

  if (code == 'A') {
    
  }
  switch (code) {
    case 'G':
    case 'R':
    case 'S': 
    break;
  } 
}

TableAction createAction(const vector<string> &results)
{
  // a primeira letra do primeiro que casou, diz qual a ação
  const char code(results[0][0]);
  int value;
  Action action;
  switch (code) {
    case 'A':
      return {ACCEPT};
    break;
    case 'G':
      action = GOTO;
    case 'R':
      action= REDUCE;
    case 'S': 
      action = SHIFT; 
    break;

    value = atoi(results[1].c_str());
    return {action,value};
  } 
}

}

// operador de serialização
template<typename SymbolT>
void operator>>(const Syntatical::Table<SymbolT> &table,fstream &file)
{
  for (auto action(table.begin()); action != table.end(); action++) {
    // coloco a ação no primeiro caractere para facilitar a identificação da ação
    file << table.actionToString(action->second._action);
    file << " ";

    if (action->second._action != Syntatical::ACCEPT) {
      file << action->second._value; 
      file << " ";
    } 
    
    file << get<0>(action->first);
    file << " ";
    file << int(get<1>(action->first)._type);
    file << " ";
    file << int(get<1>(action->first)._terminal);
    file << " ";
    file << int(get<1>(action->first)._nonTerminal);

    file << "\n";
  }
}

// operador de deserialização
template<typename SymbolT>
void operator<<(Syntatical::Table<SymbolT> &table, fstream &file)
{
  stringstream s;

  {
    // lê todo o conteúdo do arquivo para um stream
    int size(file.tellp());
    char *buffer(new char[size]);
    file.seekg(0,ios::beg);
    file.read(reinterpret_cast<char *>(buffer),size);
    s << buffer;
    delete buffer;
  }

  int curPos(0);

  char line[64];
  while (s.getline(line,64)) {
    vector<string> results;
    split(line,results,' ');
    using namespace Syntatical;
    table[createStateAndSymbol<SymbolT>(results)] = createAction(results);
  }
}

#endif
