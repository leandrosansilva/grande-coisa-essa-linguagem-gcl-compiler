#include <lexical/transitiontable.h>
#include <common/tokentype.h>
#include <lexical/filereader.h>

#include <lexical/analyser.h>
#include <syntatic/grammar.h>
#include <syntatic/analyzer.h>

#include <iostream>
#include <functional>

using namespace Lexical;
using namespace Common;
using namespace Syntatical;

using namespace std;

const String letters("abcdefghijklmnopqrstuvxwyzABCDEFGHIJKLMNOPQRSTUVXWYZ");

const String blanks(" \t");
const String breakline("\n");

const String spaces(blanks + breakline);

typedef enum {
  /* Estado inválido */
  invalid,
  
  /* Estado inicial */
  start,
  
  /* Final genérico e obrigatório, só para finalizar o autômato */
  final,
  
  /* Estados intermediários */
  a1, a2,
  sp1
} LexState;

typedef enum {
  TkId,
  TkNone,
  TkSpaces,
  TEOF,INVALID
} TokenType;

static map<TokenType,string> TerminalMap {
  {TkId,"TkId"},
  {TkSpaces,"TkSpaces"},
  {TEOF,"$"},
  {INVALID,"?"}
};

typedef enum {
  EL,
  E,
  T,
  N,
} NonTerminal;

static map<NonTerminal,string> NonTerminalMap {
  {E,"E"},
  {T,"T"},
  {N,"N"},
  {EL,"E'"} 
};

typedef Grammar<NonTerminal,TokenType> ListGrammar;

static string symbolToString(const ListGrammar::Symbol &s)
{
  if (s.isTerminal()) {
    return TerminalMap[s._terminal];
  }
  if (s.isNonTerminal()) {
    return NonTerminalMap[s._nonTerminal];
  }

  /* nunca chega aqui, asno :-) */
  if (s.isEmpty()) {
    return "$";
  }
}

ListGrammar grammar(symbolToString,{
  {EL,{{E},{TEOF}}},
  {E,{T}},
  {T,{}},
  {T,{{TkId},{T}}}
},TEOF,INVALID);

TransitionTable<LexState,TokenType> automata(start,invalid,final);

int main(int argc, char **argv)
{
  /* Consome espaços em branco */
  automata.addTransition(start,spaces,sp1);
  automata.addTransition(sp1,spaces,sp1);
  automata.addFinalTransition(sp1,letters,TkSpaces);
  
  /* Para identificador */
  automata.addTransition(start,letters,a1);
  automata.addTransition(a1,letters, a1);
  automata.addFinalTransition(a1,spaces,TkId);
  
  /* um cara que lê um arquivo do disco */
  FileReader reader(argv[1]);
  
  /* Estrutura com as palavras reservadas */
  TokenHash<TokenType> reservedWords(TkNone,{});
  
  /* Analisador léxico */
  Analyser<LexState,TokenType> lexer(reader,automata,reservedWords,TEOF);
  
  lexer.ignoreToken({TkSpaces});
  
  function<Token<TokenType>()> getToken([&lexer](){
    return lexer.getToken();
  });

  Syntatical::Analyzer<NonTerminal,TokenType> parser(grammar,getToken);

  grammar.printTable();

  grammar.generateGraph();

  if (!parser.parse()) {
    cerr << "Erro!" << endl;
    return 1;
  }

  Tree<TokenType,ListGrammar::Symbol> tree(parser.getTree());

  cerr << tree.toString<function<string(const ListGrammar::Symbol &)>>(symbolToString) << endl;

  tree.dispose();

  return 0;
}
