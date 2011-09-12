#include <lexical/transitiontable.h>
#include <common/tokentype.h>
#include <lexical/filereader.h>
#include <lexical/analyser.h>

#include <iostream>

using namespace Lexical;
using namespace Common;

using namespace std;

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
} State;

typedef enum {
  /* Tokens de teste */
  TkId,
  
  TkString,
  TkInteger,
  TkReal,
  
  TkComment,
  TkSpaces,
  
  TkAssign,
  TkSymbol,
  TkThen,
  TkEnd,
  TkTwoDots,
  TkDot,
  TkRBracket,
  TkLBracket,
  TkGuarded,
  TkLThan,
  TkGThan,
  TkLEThan,
  TkGEThan,
  TkLParentesis,
  TkRParentesis,
  TkSharp,
  TkComma,
  TkEqual,
  
  /* Lógicos */
  TkAnd,
  TkOr,
  TkNot,
  
  /* Operações matemáticas */
  TkMinus,
  TkPlus,
  TkTimes,
  TkDiv,
  TkRem,
  
  /* Palavras reservadas */
  TkModule,
  TkPrivate,
  TkEndWord,
  TkConst,
  TkBoolean,
  TkIntegerWord,
  TkRealWord,
  TkBegin,
  TkTypedef,
  TkArray,
  TkRange,
  TkProc,
  TkVal,
  TkRef,
  TkReturn,
  TkWrite,
  TkRead,
  TkIf,
  TkFi,
  TkDo,
  TkOd,
  TkTrue,
  TkFalse,
  TkForall,
  TkLlarof,
  TkSkip,
  
  TkNone
} TokenType;

map<TokenType,string> tokenToString {
  /* Tokens de teste */
  {TkId,"TkId"},
    
  {TkString,"TkString"},
  {TkInteger,"TkInteger"},
  {TkReal,"TkReal"},
    
  {TkComment,"TkComment"},
  {TkSpaces,"TkSpaces"},
    
  {TkAssign,"TkAssign"},
  {TkSymbol,"TkSymbol"},
  {TkThen,"TkThen"},
  {TkEnd,"TkEnd"},
  {TkTwoDots,"TkTwoDots"},
  {TkDot,"TkDot"},
  {TkRBracket,"TkRBracket"},
  {TkLBracket,"TkLBracket"},
  {TkGuarded,"TkGuarded"},
  {TkLThan,"TkLThan"},
  {TkGThan,"TkGThan"},
  {TkLEThan,"TkLEThan"},
  {TkGEThan,"TkGEThan"},
  {TkLParentesis,"TkLParentesis"},
  {TkRParentesis,"TkRParentesis"},
  {TkSharp,"TkSharp"},
  {TkComma,"TkComma"},
  {TkEqual,"TkEqual"},
    
  /* Lógicos */
  {TkAnd,"TkAnd"},
  {TkOr,"TkOr"},
  {TkNot,"TkNot"},
    
  /* Operações matemáticas */
  {TkMinus,"TkMinus"},
  {TkPlus,"TkPlus"},
  {TkTimes,"TkTimes"},
  {TkDiv,"TkDiv"},
  {TkRem,"TkRem"},
    
  /* Palavras reservadas */
  {TkModule,"TkModule"},
  {TkPrivate,"TkPrivate"},
  {TkEndWord,"TkEndWord"},
  {TkConst,"TkConst"},
  {TkBoolean,"TkBoolean"},
  {TkIntegerWord,"TkIntegerWord"},
  {TkRealWord,"TkRealWord"},
  {TkBegin,"TkBegin"},
  {TkTypedef,"TkTypedef"},
  {TkArray,"TkArray"},
  {TkRange,"TkRange"},
  {TkProc,"TkProc"},
  {TkVal,"TkVal"},
  {TkRef,"TkRef"},
  {TkReturn,"TkReturn"},
  {TkWrite,"TkWrite"},
  {TkRead,"TkRead"},
  {TkIf,"TkIf"},
  {TkFi,"TkFi"},
  {TkDo,"TkDo"},
  {TkOd,"TkOd"},
  {TkTrue,"TkTrue"},
  {TkFalse,"TkFalse"},
  {TkForall,"TkForall"},
  {TkLlarof,"TkLlarof"},
  {TkSkip,"TkSkip"},
    
  {TkNone,"TkNone"} 
};
  
int main(int argc, char **argv)
{
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
  
  TransitionTable<State,TokenType> automata(start,invalid,final);
  
  /* Consome espaços em branco */
  automata.addTransition(start,spaces,sp1);
  automata.addTransition(sp1,spaces,sp1);
  automata.addFinalTransition(sp1,any - spaces,TkSpaces);
  
  /* Para inteiros e reais */
  automata.addTransition(start,digits,b1);
  automata.addTransition(b1,digits,b1);
  automata.addFinalTransition(b1,any - digits - ".",TkInteger);
  automata.addTransition(b1,".",b2);
  automata.addTransition(b2,digits,b3);
  automata.addTransition(b3,digits,b3);
  automata.addFinalTransition(b3,any - digits,TkReal);
  automata.addTransition(b2,".",b4);
  
  /* se para chegar a b4 eu passei por um b1 há dois caracteres atrás,
   * b1 era um número, então devo casá-lo, e não casar os dois pontos 
  */
  automata.addConflict(b1,b4);
  
  /* Para identificador */
  automata.addTransition(start,letters,a1);
  automata.addTransition(a1,letters + digits, a1);
  automata.addTransition(a1,"_",a2);
  automata.addTransition(a2,letters + digits, a1);
  automata.addFinalTransition(a1,any - letters - digits,TkId);
  automata.addFinalTransition(a2,any - letters - digits,TkId);
  
  /* Para [, ] e [] */
  automata.addTransition(start,"[",g1);
  automata.addTransition(g1,"]",g2);
  automata.addFinalTransition(g1,any,TkLBracket);
  automata.addFinalTransition(g2,any,TkGuarded);
  automata.addTransition(start,"]",l1);
  automata.addFinalTransition(l1,any,TkRBracket);
  
  /* Para < e <= */
  automata.addTransition(start, "<", h1);
  automata.addTransition(h1,"=",h2);
  automata.addFinalTransition(h1,any,TkLThan);
  automata.addFinalTransition(h2,any,TkLEThan);
  
  /* Para > e >= */
  automata.addTransition(start, ">", i1);
  automata.addTransition(i1,"=",i2);
  automata.addFinalTransition(i1,any,TkGThan);
  automata.addFinalTransition(i2,any,TkGEThan);
  
  /* Para parentesis */
  automata.addTransition(start,"(",m1);
  automata.addFinalTransition(m1,any,TkLParentesis);
  automata.addTransition(start,")",m2);
  automata.addFinalTransition(m2,any,TkRParentesis);
  
  /* Para = */
  automata.addTransition(start,"=",n1);
  automata.addFinalTransition(n1,any,TkEqual);
  
  /* Para , */
  automata.addTransition(start,",",o1);
  automata.addFinalTransition(o1,any,TkComma);
  
  /* Para + */
  automata.addTransition(start,"+",p1);
  automata.addFinalTransition(p1,any,TkPlus);
  
  /* Para # */
  automata.addTransition(start,"#",q1);
  automata.addFinalTransition(q1,any,TkSharp);
  
  /* Para ~ */
  automata.addTransition(start,"~",r1);
  automata.addFinalTransition(r1,any,TkNot);
  
  /* Para & */
  automata.addTransition(start,"&",s1);
  automata.addFinalTransition(s1,any,TkAnd);
  
  /* Para | */
  automata.addTransition(start,"|",t1);
  automata.addFinalTransition(t1,any,TkOr);
  
  /* Para / */
  automata.addTransition(start,"/",u1);
  automata.addFinalTransition(u1,any,TkDiv);
  
  /* Para \ */
  automata.addTransition(start,"\\",v1);
  automata.addFinalTransition(v1,any,TkRem);
  
  /* Para * */
  automata.addTransition(start,"*",w1);
  automata.addFinalTransition(w1,any,TkTimes);
  
  /* Para .. e . */
  automata.addTransition(start,".",f1);
  automata.addTransition(f1,".",f2);
  
  automata.addFinalTransition(f2,any - ".",TkTwoDots);
  automata.addFinalTransition(f1,any - ".", TkDot);
  
  /* separador de comandos ponto e vírgula (;) */
  automata.addTransition(start,";",k1);
  automata.addFinalTransition(k1,any,TkEnd);
  
  /* string com aspas simples*/
  automata.addTransition(start,"\'",c1);
  automata.addTransition(c1,any - "\'",c1);
  automata.addTransition(c1,"\'",c3);

  /* string com aspas duplas */
  automata.addTransition(start,"\"",c2);
  automata.addTransition(c2,any - "\"",c2);
  automata.addTransition(c2,"\"",c3);
  automata.addFinalTransition(c3,any,TkString);
  
  /* Para atribuição */
  automata.addTransition(start,":",e1);
  automata.addTransition(e1,"=",e2);
  automata.addFinalTransition(e2,any,TkAssign);
  
  /* Para comentário */
  automata.addTransition(start,"-",d1);
  automata.addTransition(d1,"-",d3);
  automata.addTransition(d3,any - breakline,d3);
  automata.addFinalTransition(d3,breakline,TkComment);
  
  /* para símbolo - (minus) */
  automata.addFinalTransition(d1,any - ">-",TkMinus);
  
  /* para símbolo -> */
  automata.addTransition(d1,">",d2);
  automata.addFinalTransition(d2,any,TkThen);
  
  /* um cara que lê um arquivo do disco */
  FileReader reader(argv[1]);
  
  /* Estrutura com as palavras reservadas */
  TokenHash<TokenType> reservedWords(
    TkNone,{
      {"module",TkModule},
      {"private",TkPrivate},
      {"end",TkEndWord},
      {"const",TkConst},
      {"Boolean",TkBoolean},
      {"integer",TkIntegerWord},
      {"real",TkRealWord},
      {"begin",TkBegin},
      {"typedef",TkTypedef},
      {"array",TkArray},
      {"range",TkRange},
      {"proc",TkProc},
      {"val",TkVal},
      {"ref",TkRef},
      {"return",TkReturn},
      {"write",TkWrite},
      {"read",TkRead},
      {"if",TkIf},
      {"fi",TkFi},
      {"do",TkDo},
      {"od",TkOd},
      {"true",TkTrue},
      {"false",TkFalse},
      {"forall",TkForall},
      {"llarof",TkLlarof},
      {"skip",TkSkip} 
    });
  
  /* Analisador léxico */
  Analyser<State,TokenType> lexer(reader,automata,reservedWords);
  
  // os identificadores devem ser comparados na tabela de palavras reservadas
  lexer.addTokenToCompareWithReserved({TkId});
  
  /* Ignore os seguintes tokens,
   * que não serão passados pro
   * analisador sintático: espaços e comentários
  */
  lexer.ignoreToken({TkSpaces,TkComment});
  
  /* 
   * todo token do tipo string terá "cortados" um caractere à esquerda e outro à direita
   * estes caracteres correspondem às aspas e apóstrofos
   */
  lexer.setTokenPadding(TkString,1,1);
  
  /* Laço principal do analisador sintático, ainda não implementado */
  while (lexer.canReadToken())
  {
    Token<TokenType> t(lexer.getToken());
    std::cout << "'" << t.getLexema() << "' => "
              << tokenToString[t.getType()] << " na linha: " << t.getLine() << std::endl;
  }
  
  return 0;
}
