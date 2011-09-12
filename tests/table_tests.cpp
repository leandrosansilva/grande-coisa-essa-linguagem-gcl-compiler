#include <table.h>

#include <string>

int main(int argc, char **argv)
{
  Table<string> table;

  typedef tuple<int,string> Key; 

  table[Key(4,"tr")] = {REDUCE,90};
  table[Key(1,"a")] = {REDUCE,1};
  table[Key(1,"b")] = {GOTO,2};
  table[Key(1,"c")] = {ACCEPT};
  table[Key(1,"d")] = {SHIFT,3};
  table[Key(1,"e")] = {ERROR};
  table[Key(3,"Manoel")] = {REDUCE,5};

  for (auto e(table.begin()); e != table.end(); e++) {
    cout << get<0>(e->first) << " " << get<1>(e->first) 
         << " " << e->second._action << " " << e->second._state << endl;
  }
}
