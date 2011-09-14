#ifndef MYGRAMMAR_H
#define MYGRAMMAR_H

#include <src/syntatic/grammar.h>
#include <src/syntatic/automata.h>

using namespace Syntatical;

typedef enum
{
  EL,E, T, F,
  
  D,K,B,H,
  
  SL,S,V
} NonTerminal;

typedef enum
{
  IF,THEN,ELSE,LPAR,RPAR,ID,
  PLUS,TIMES,EQUAL,ATTR,
  TEOF,INVALID
} Terminal;

static map<Terminal,string> TerminalMap 
{
  {IF,"if"},
  {THEN,"then"},
  {ELSE,"else"},
  {LPAR,"("},
  {RPAR,")"},
  {ID,"x"},
  {PLUS,"+"},
  {TIMES,"*"},
  {EQUAL,"=="},
  {ATTR,"="},
  {TEOF,"$"},
  {INVALID,"?"}
};

static map<NonTerminal,string> NonTerminalMap 
{
  {EL,"E'"},
  {E,"E"},
  {T,"T"},
  {F,"F"},
  {D,"D"},
  {K,"K"},
  {B,"B"},
  {H,"H"},
  {SL,"S'"},
  {S,"S"},
  {V,"V"}
};

typedef Grammar<NonTerminal,Terminal> MyGrammar;
typedef Automata<NonTerminal,Terminal> MyAutomata;

string symbolToString(const MyGrammar::Symbol &s)
{
  if (s.isTerminal()) {
    return TerminalMap[s._terminal];
  }
  if (s.isNonTerminal()) {
    return NonTerminalMap[s._nonTerminal];
  }

  if (s.isEmpty()) {
    return "$";
  }
}

#endif
