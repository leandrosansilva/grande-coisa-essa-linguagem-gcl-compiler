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

/* defino o conjunto de caracteres permitidos na linguagem */
const String digits("0123456789");
const String letters("abcdefghijklmnopqrstuvxwyzABCDEFGHIJKLMNOPQRSTUVXWYZ");
const String symbols("+-*/=");

/* aspas (quotes) e apóstrofos (apostrophes) */
const String quotes("\'\"");

const String blanks(" \t");
const String breakline("\n");

const String spaces(blanks + breakline);

const String separators(spaces + "()");

/* qualquer caractere! */
const String any(digits + letters + separators + quotes + symbols);

typedef enum {
  /* Estado inválido */
  invalid,
  
  /* Estado inicial */
  start,
  
  /* Final genérico e obrigatório, só para finalizar o autômato */
  final,
  
  /* Estados intermediários */
  a1, a2,
  b1, b2, b3, b4,
  c1, c2, c3,
  sp1
} LexState;

typedef enum {
  TkId,
  TkString,
  TkNone,
  TkLPar,
  TkRPar,
  TkSpaces,
  TEOF,INVALID
} TokenType;

static map<TokenType,string> TerminalMap {
  {TkId,"TkId"},
  {TkString,"TkString"},
  {TkLPar,"TkLPar"},
  {TkRPar,"TkRPar"},
  {TkNone,"TkNone"},
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

typedef Grammar<NonTerminal,TokenType> LISPGrammar;

static string symbolToString(const LISPGrammar::Symbol &s)
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

LISPGrammar grammar(symbolToString,{
  {EL,{{E},{TEOF}}},
  {E,{{TkLPar},{T},{TkRPar}}},
  {T,{{N},{T}}},
  {T,{}},
  {N,{{TkId}}},
  {N,{{TkString}}},
  {N,{{E}}}
},TEOF,INVALID);

TransitionTable<LexState,TokenType> automata(start,invalid,final);

int main(int argc, char **argv)
{
  /* Consome espaços em branco */
  automata.addTransition(start,spaces,sp1);
  automata.addTransition(sp1,spaces,sp1);
  automata.addFinalTransition(sp1,any - spaces,TkSpaces);
  
  /* Para identificador */
  automata.addTransition(start,any - spaces - "\"()",a1);
  automata.addTransition(a1,any - spaces - "\"()", a1);
  automata.addFinalTransition(a1,spaces + "\"()",TkId);
  
  /* Para parentesis */
  automata.addTransition(start,"(",b1);
  automata.addFinalTransition(b1,any,TkLPar);
  automata.addTransition(start,")",b2);
  automata.addFinalTransition(b2,any,TkRPar);
  
  /* string */
  automata.addTransition(start,"\"",c2);
  automata.addTransition(c2,any - "\"",c2);
  automata.addTransition(c2,"\"",c3);
  automata.addFinalTransition(c3,any,TkString);
  
  /* um cara que lê um arquivo do disco */
  FileReader reader(argv[1]);
  
  /* Estrutura com as palavras reservadas */
  TokenHash<TokenType> reservedWords(TkNone,{});
  
  /* Analisador léxico */
  Lexical::Analyser<LexState,TokenType> lexer(reader,automata,reservedWords,TEOF);
  
  /* Ignore os seguintes tokens,
   * que não serão passados pro
   * analisador sintático: espaços e comentários
  */
  lexer.ignoreToken({TkSpaces});
  
  /* 
   * todo token do tipo string terá "cortados" um caractere à esquerda e outro à direita
   * estes caracteres correspondem às aspas e apóstrofos
   */
  lexer.setTokenPadding(TkString,1,1);

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

  Tree<TokenType,LISPGrammar::Symbol> tree(parser.getTree());

  cerr << tree.toString<function<string(const LISPGrammar::Symbol &)>>(symbolToString) << endl;

  tree.dispose();

  return 0;
}
