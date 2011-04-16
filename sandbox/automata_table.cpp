#include <lexical/transitiontable.h>
#include <common/tokentype.h>
#include <lexical/filereader.h>
#include <lexical/analyser.h>

using namespace Lexical;
using namespace Common;

typedef enum {
  /* Estado inválido */
  invalid = -1,
  
  /* Estado inicial */
  start,
  
  /* Final genérico e obrigatório, só para finalizar o autômato */
  final,
  
  /* Estados intermediários */
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
  sp1 = 100
} State;

typedef String TokenType;

/* Tokens de teste */
TokenType
  TkId("Id"),
  TkString("String"),
  TkInteger("Integer"),
  TkReal("Real"),
  TkComment("Comment"),
  TkSpaces("Spaces"),
  TkAssign("Assign"),
  TkSymbol("Symbol"),
  TkIf("If"),
  TkFor("For"),
  TkNone("None");
  
int main(int argc, char **argv)
{
  const String digits("0123456789");
  const String letters("abcdefghijklmnopqrstuvxwyzABCDEFGHIJKLMNOPQRSTUVXWYZ");
  const String symbols(":.=-,;()[]#<>&|~+-*/\\.");
  
  /* aspas (quotes) e apóstrofos (apostrophes) */
  const String quotes("\'\"");
  
  const String blanks(" \t"); // tem tbm \t
  const String breakline("\n");
  
  const String spaces(blanks + breakline);
  
  const String separators(symbols + spaces);
  
  TransitionTable<State,TokenType> automata(start,invalid,final);
  
  /* Consome espaços em branco */
  automata.addTransition(start,spaces,sp1);
  automata.addTransition(sp1,spaces,sp1);
  automata.addFinalTransition(sp1,letters + digits + quotes + symbols,TkSpaces);
  
  /* Para inteiros e reais */
  automata.addTransition(start,digits,b1);
  automata.addTransition(b1,".",b2);
  automata.addTransition(b1,digits,b1);
  automata.addTransition(b2,digits,b3);
  automata.addTransition(b3,digits,b3);
  automata.addFinalTransition(b3,separators + quotes,TkReal);
  automata.addFinalTransition(b1,separators + quotes,TkInteger);
  
  /* Para identificador */
  automata.addTransition(start,letters,a1);
  automata.addTransition(a1,letters + digits, a1);
  automata.addTransition(a1,"_",a2);
  automata.addTransition(a2,letters + digits, a1);
  automata.addFinalTransition(a1,separators + quotes,TkId);
  automata.addFinalTransition(a2,separators + quotes,TkId);
  
  /* string com aspas simples*/
  automata.addTransition(start,"\'",c1);
  automata.addTransition(c1,letters + digits + blanks + symbols,c1);
  automata.addTransition(c1,"\'",c3);

  /* string com aspas duplas */
  automata.addTransition(start,"\"",c2);
  automata.addTransition(c2,letters + digits + blanks + symbols,c2);
  automata.addTransition(c2,"\"",c3);
  automata.addFinalTransition(c3,separators + digits + letters,TkString);
  
  /* Para atribuição */
  automata.addTransition(start,":",e1);
  automata.addTransition(e1,"=",e2);
  automata.addFinalTransition(e2,digits + letters + separators + quotes,TkAssign);
  
  /* Para comentário */
  automata.addTransition(start,"-",d1);
  automata.addTransition(d1,"-",d3);
  automata.addTransition(d3,digits + letters + symbols + blanks,d3);
  automata.addFinalTransition(d3,breakline,TkComment);
  
  /* para símbolo - (minus) */
  automata.addFinalTransition(d1,letters + digits + spaces,TkSymbol);
  
  /* para símbolo -> */
  automata.addTransition(d1,">",d2);
  automata.addFinalTransition(d2,letters + digits + spaces,TkSymbol);
  
  FileReader reader(argv[1]);
  
  /* Estrutura com as palavras reservadas */
  TokenHash<TokenType> reservedWords(TkNone);
  reservedWords.add("if",TkIf);
  reservedWords.add("for",TkFor);
  
  Analyser<State,TokenType> analyser(reader,automata,reservedWords);
  
  //id deve ser comparado na tabela de palavras reservadas
  analyser.addTokenToCompareWithReserved(TkId);
  
  /* Ignore os seguintes tokens,
   * que não serão passados pro 
   * analisador sintático: espaços e comentários
  */
  analyser.ignoreToken(TkSpaces);
  analyser.ignoreToken(TkComment);
  
  while (analyser.canReadToken())
  {
    Token<TokenType> t(analyser.getToken());
    
    if (t.getType() == TkNone) {
      std::cout << "Peguei um token inválido. Poutz! Casei só '" 
                << t.getLexema() << "'"
                << std::endl;
    } else {
    
      std::cout << "'" << t.getLexema() << "' que é do tipo "
                << t.getType() << " e está em "
                << t.getLine() << "x" << t.getColumn() << std::endl;
    }
  }
  
  return 0;
}
