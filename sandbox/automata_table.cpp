/*
 * tabela
*/
#include <ctype.h>
#include <string>
#include <vector>
#include <iostream>

typedef enum {
  /* Estado inválido */
  invalid = 0,
  
  /* Estado inicial */
  start,
  
  a1, a2,
  b1, b2, b3,
  c1, c2, c3,
  d1, d2, d3,
  e1, e2,
  f1, f2,
  g1, g2,
  h1, h2,
  i1, i2,
  j1,
  
  /* Estados finais */
  final_id,
  final_number,
  final_real
} State;

class String: public std::string
{
public:
  String(const char *str): std::string(str)
  {
  }
  
  String(): std::string() 
  {
  }
  
  bool hasChar(char c)
  {
    return find(c) != npos;
  }
};

const String digits("0123456789");
const String letters("abcdefghijklmnopqrstuvxwyzABCDEFGHIJKLMNOPQRSTUVXWYZ");
const String symbols(".=-,;()[]#<>&|~+-*/\\.\'\"");

class Transition
{
public:
  State _from;
  State _to;
  String _pattern;
  
  Transition(State from, State to, const String symbols):
  _from(from),
  _to(to),
  _pattern(symbols)
  {
  }
};

/* FIXME: implementação com array porca.
 * TODO: reimplementar de forma mais eficiente 
*/
class TransitionTable: public std::vector<Transition>
{
public:
  void registerTransition(State from, const String symbols, State to)
  {
    push_back(Transition(from,to,symbols));
  }
  
  State doTransition(State from, char symbol)
  {
    /* Acha o elemento */
    iterator i = begin();
    for (; i != end(); i++) {
      if (i->_from == from && i->_pattern.hasChar(symbol))
        break;
    }
    
    return i != end() ? i->_to : invalid;
  }
};

TransitionTable table;

int main(int argc, char **argv)
{
  table.registerTransition(start,digits,b1);
  table.registerTransition(b1,".",b2);
  table.registerTransition(b1,digits,b1);
  table.registerTransition(b2,digits,b3);
  table.registerTransition(b3,digits,b3);
  
  table.registerTransition(b2,symbols,final_real);
  table.registerTransition(b1,symbols,final_number);
  
  String input(argv[1]);
  String match;
  
  State s(start);
  for (int i = 0; i<input.size(); i++) {
    s = table.doTransition(s,input[i]);
    
    if (s) {
      switch (s) {
        case final_number:
          std::cout << "Casou número " << match << std::endl;
        break;
        case final_real:
          std::cout << "Casou real " << match << std::endl;
        break;
      }
      
      match += input[i];
    } else
      break;
  }
  
  return 0;
}