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
  final_real,
  final_string,
  final_assign
} State;

typedef enum {
  Invalido = 0,
  Inicial,
  Estado1,
  EstadoFinal
} State2;

class String: public std::string
{
public:
  String(const char *str): std::string(str)
  {
  }
  
  String(const String &other): std::string(other)
  {
  }
  
  String(std::basic_string<char,std::char_traits<char>,std::allocator<char> > other): std::string(other)
  {
  }
  
  String(): std::string() 
  {
  }
  
  const String operator+(const String &o1)
  {
  }
  
  bool hasChar(char c)
  {
    return find(c) != npos;
  }
};

template<typename T>
class Transition
{
public:
  T _from;
  T _to;
  String _pattern;
  
  Transition(T from, T to, const String &s):
  _from(from),
  _to(to),
  _pattern(s)
  {
  }
};

/* FIXME: implementação com array porca.
 * TODO: reimplementar de forma mais eficiente
 * 
*/
template<typename T>
class TransitionTable
{
  typedef std::vector< Transition<T> > vector;
  
  T _start;
  T _invalid;
  vector _table;
  
public:
  /* Please tell me the initial state! */
  TransitionTable(T start, T invalid): 
  _start(start),
  _invalid(invalid)
  {
  }
  
  void addTransition(T from, const String &s, T to)
  {
    _table.push_back(Transition<T>(from,to,s));
  }
  
  T doTransition(T from, char symbol)
  {
    /* Acha o elemento */
    typename vector::iterator i(_table.begin());
    for (; i != _table.end(); i++) {
      if (i->_from == from && i->_pattern.hasChar(symbol))
        break;
    }
    
    return i != _table.end() ? i->_to : _invalid;
  }
  
  void reset(T &s, String &match, int &i)
  {
    s = _start;
    match = "";
    //i--;
  }
};

int main(int argc, char **argv)
{
  const String digits("0123456789");
  const String letters("abcdefghijklmnopqrstuvxwyzABCDEFGHIJKLMNOPQRSTUVXWYZ");
  const String symbols(".=-,;()[]#<>&|~+-*/\\.");
  
  /* aspas (quotes) e apóstrofos (apostrophes) */
  const String quotes("\'\"");
  
  const String blanks("\t\b ");
  const String breakline("\n");
  const String separators(symbols + blanks + breakline);
  
  TransitionTable<State> table(start,invalid);
  
  /* Para inteiros e reais */
  table.addTransition(start,digits,b1);
  table.addTransition(b1,".",b2);
  table.addTransition(b1,digits,b1);
  table.addTransition(b2,digits,b3);
  table.addTransition(b3,digits,b3);
  table.addTransition(b3,separators + quotes,final_real);
  table.addTransition(b1,separators + quotes,final_number);
  
  /* Para identificador */
  table.addTransition(start,letters,a1);
  table.addTransition(a1,letters + digits, a1);
  table.addTransition(a1,"_",a2);
  table.addTransition(a2,letters + digits, a1);
  table.addTransition(a1,separators + quotes,final_id);
  table.addTransition(a2,separators + quotes,final_id);
  
  /* string com aspas simples*/
  table.addTransition(start,"\'",c1);
  table.addTransition(c1,letters + digits + blanks,c1);
  table.addTransition(c1,"\'",c3);
  table.addTransition(c3,separators + digits + letters,final_string);
  
  /* string com aspas duplas */
  table.addTransition(start,"\"",c2);
  table.addTransition(c2,letters + digits + blanks,c2);
  table.addTransition(c2,"\"",c3);
  
  /* Para atribuição */
  table.addTransition(start,":",e1);
  table.addTransition(e1,"=",e2);
  table.addTransition(e2,digits + letters + separators + quotes,final_assign);
  
  String input(argv[1]);
  String match;
  
  State s(start);
  for (int i = 0; i<input.size();) {
    s = table.doTransition(s,input[i]);
    
    if (s) {
      switch (s) {
        case final_number:
          std::cout << "Casou número " << match << std::endl;
          table.reset(s,match,i);
        break;
        case final_real:
          std::cout << "Casou real " << match << std::endl;
          table.reset(s,match,i);
        break;
        case final_id:
          std::cout << "Casou id " << match << std::endl;
          table.reset(s,match,i);
        break;
        case final_string:
          std::cout << "Casou string " << match << std::endl;
          table.reset(s,match,i);
        break;
        case final_assign:
          std::cout << "Casou assign " << match << std::endl;
          table.reset(s,match,i);
        break;
      }
      
      match += input[i];
      i++;
    } else
      break;
  }
  
  return 0;
}