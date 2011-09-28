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
const String symbols(":.=-,;()[]#<>&|~+-*/\\.");

/* outros caracteres que podem aparecer em textos corridos */
const String others("!@$%{}");

/* aspas (quotes) e apóstrofos (apostrophes) */
const String quotes("\'\"");

const String blanks(" \t");
const String breakline("\n");

const String spaces(blanks + breakline);

const String separators(symbols + spaces);

/* qualquer caractere! */
const String any(digits + letters + separators + quotes + others);

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
  d1, d2, d3,
  e1, e2,
  f1, f2,
  g1, g2,
  h1, h2,
  i1, i2,
  J1,
  k1,
  l1,
  m1,m2,
  n1,
  o1,
  p1,
  q1,
  r1,
  s1,
  t1,
  u1,
  v1,
  w1,
  sp1
} LexState;

typedef enum {
  TkId,
  TkAssign,
  TkIf,
  TkElse,
  TkEnd,
  TkLPar,
  TkRPar,
  TkPlus,
  TkTimes,
  TkSpaces,
  TkTrue,
  TkFalse,
  TkNone,
  TEOF,
  INVALID
} TokenType;

static map<TokenType,string> TerminalMap {
  {TkId,"Id"},
  {TkAssign,":="},
  {TkIf,"if"},
  {TkElse,"else"},
  {TkEnd,";"},
  {TkLPar,"("},
  {TkRPar,")"},
  {TkPlus,"+"},
  {TkTimes,"*"},
  {TkSpaces,"<space>"},
  {TkTrue,"true"},
  {TkFalse,"false"},

  {TkNone,"<error>"},
  {TEOF,"$"},
  {INVALID,"?"}
};

typedef enum {
  EL,
  Program,
  StmList,
  AssStm,
  Stm,
  IfStm,
  Condition
} NonTerminal;

static map<NonTerminal,string> NonTerminalMap {
  {EL,"E'"}, 
  {Program,"Program"},
  {StmList,"StmList"},
  {AssStm,"AssStm"},
  {Stm,"Stm"},
  {IfStm,"IfStm"},
  {Condition,"Condition"},
};

typedef Grammar<NonTerminal,TokenType> AmbGrammar;

static string symbolToString(const AmbGrammar::Symbol &s)
{
  if (s.isTerminal()) {
    return TerminalMap[s._terminal];
  }

  if (s.isNonTerminal()) {
    return NonTerminalMap[s._nonTerminal];
  }

  return "$";
}

TransitionTable<LexState,TokenType> automata(start,invalid,final);

AmbGrammar ifG(symbolToString,{
  {EL,{{Program},{TEOF}}},
  {Program,{{StmList}}},
  {StmList,{}},
  {StmList,{{Stm},{StmList}}},
  {Stm,{{IfStm}}},
  {Stm,{{AssStm}}},
  {AssStm,{{TkId},{TkAssign},{TkId}},{0,2}},
  {IfStm,{{TkIf},{TkLPar},{Condition},{TkRPar},{StmList}},{2,4}},
  {IfStm,{{TkIf},{TkLPar},{Condition},{TkRPar},{StmList},{TkElse},{StmList}},{2,4,6}},
  {Condition,{{TkTrue}}},
  {Condition,{{TkFalse}}}
},TEOF,INVALID);

int main(int argc, char **argv)
{
  /* Consome espaços em branco */
  automata.addTransition(start,spaces,sp1);
  automata.addTransition(sp1,spaces,sp1);
  automata.addFinalTransition(sp1,any - spaces,TkSpaces);
  
  /* Para identificador */
  automata.addTransition(start,letters,a1);
  automata.addTransition(a1,letters + digits, a1);
  automata.addTransition(a1,"_",a2);
  automata.addTransition(a2,letters + digits, a1);
  automata.addFinalTransition(a1,any - letters - digits,TkId);
  automata.addFinalTransition(a2,any - letters - digits,TkId);
  
  /* Para parentesis */
  automata.addTransition(start,"(",m1);
  automata.addFinalTransition(m1,any,TkLPar);
  automata.addTransition(start,")",m2);
  automata.addFinalTransition(m2,any,TkRPar);
  
  /* Para + */
  automata.addTransition(start,"+",p1);
  automata.addFinalTransition(p1,any,TkPlus);
  
  /* Para * */
  automata.addTransition(start,"*",w1);
  automata.addFinalTransition(w1,any,TkTimes);
  
  /* separador de comandos ponto e vírgula (;) */
  automata.addTransition(start,";",k1);
  automata.addFinalTransition(k1,any,TkEnd);
  
  /* Para atribuição */
  automata.addTransition(start,":",e1);
  automata.addTransition(e1,"=",e2);
  automata.addFinalTransition(e2,any,TkAssign);
  
  /* um cara que lê um arquivo do disco */
  FileReader reader(argv[1]);
  
  /* Estrutura com as palavras reservadas */
  TokenHash<TokenType> reservedWords(
    TkNone,{
      {"if",TkIf},
      {"else",TkElse},
      {"true",TkTrue},
      {"false",TkFalse}
    }
  );
  
  /* Analisador léxico */
  Analyser<LexState,TokenType> lexer(reader,automata,reservedWords,TEOF);
  
  // os identificadores devem ser comparados na tabela de palavras reservadas
  lexer.addTokenToCompareWithReserved({TkId});
  
  lexer.ignoreToken({TkSpaces});
  
  function<Token<TokenType>()> getToken([&lexer](){
    return lexer.getToken();
  });

  Syntatical::Analyzer<NonTerminal,TokenType> parser(ifG,getToken);

  //ifG.printTable();

  //ifG.generateGraph();

  if (!parser.parse()) {
    cerr << "Erro!" << endl;
    return 1;
  }

  Tree<TokenType,AmbGrammar::Symbol> tree(parser.getTree());

  //cerr << tree.toString<function<string(const AmbGrammar::Symbol &)>>(symbolToString) << endl;

  tree.generateGraph<function<string(const AmbGrammar::Symbol &)>>(symbolToString);

  tree.dispose();

  return 0;
}
