#include <src/lexical/transitiontable.h>
#include <src/lexical/tokentype.h>

using namespace Lexical;
String
  /* Estado inválido */
  invalid("invalid"),
  
  /* Estado inicial */
  start("start"),
  
  a1("a1"), a2("a2"),
  b1("b1"), b2("b2"), b3("b3"),
  c1("c1"), c2("c2"), c3("c3"),
  d1("d1"), d2("d2"), d3("d3"),
  e1("e1"), e2("e2"),
  f1("f1"), f2("f2"),
  g1("g1"), g2("g2"),
  h1("h1"), h2("h2"),
  i1("i1"), i2("i2"),
  j1("j1"),
  
  /* Para espaço */
  sp1("sp1"),
  
  /* Final genérico */
  final("final");
  
  /* Tokens de teste */
  String 
  TkId("Id"),
  TkString("String"),
  TkInteger("Integer"),
  TkReal("Real"),
  TkComment("Comment"),
  TkSpaces("Spaces"),
  TkAssign("Assign")
  ;
  
int main(int argc, char **argv)
{
  const String digits("0123456789");
  const String letters("abcdefghijklmnopqrstuvxwyzABCDEFGHIJKLMNOPQRSTUVXWYZ");
  const String symbols(":.=-,;()[]#<>&|~+-*/\\.");
  
  /* aspas (quotes) e apóstrofos (apostrophes) */
  const String quotes("\'\"");
  
  const String blanks("\t ");
  const String breakline("\n");
  
  const String spaces(blanks + breakline);
  
  const String separators(symbols + spaces);
  
  /* Estrutura com as palavras reservadas */
  TokenHash reservedWords;
  reservedWords["if"] = If;
  
  TransitionTable<String,String> table(start,invalid,final);
  
  /* Consome espaços */
  table.addTransition(start,spaces,sp1);
  table.addTransition(sp1,spaces,sp1);
  table.addFinalTransition(sp1,letters + digits + quotes + symbols,TkSpaces);
  
  /* Para inteiros e reais */
  table.addTransition(start,digits,b1);
  table.addTransition(b1,".",b2);
  table.addTransition(b1,digits,b1);
  table.addTransition(b2,digits,b3);
  table.addTransition(b3,digits,b3);
  table.addFinalTransition(b3,separators + quotes,TkReal);
  table.addFinalTransition(b1,separators + quotes,TkInteger);
  
  /* Para identificador */
  table.addTransition(start,letters,a1);
  table.addTransition(a1,letters + digits, a1);
  table.addTransition(a1,"_",a2);
  table.addTransition(a2,letters + digits, a1);
  table.addFinalTransition(a1,separators + quotes,TkId);
  table.addFinalTransition(a2,separators + quotes,TkId);
  
  /* string com aspas simples*/
  table.addTransition(start,"\'",c1);
  table.addTransition(c1,letters + digits + blanks,c1);
  table.addTransition(c1,"\'",c3);

  /* string com aspas duplas */
  table.addTransition(start,"\"",c2);
  table.addTransition(c2,letters + digits + blanks,c2);
  table.addTransition(c2,"\"",c3);
  table.addFinalTransition(c3,separators + digits + letters,TkString);
  
  /* Para atribuição */
  table.addTransition(start,":",e1);
  table.addTransition(e1,"=",e2);
  table.addFinalTransition(e2,digits + letters + separators + quotes,TkAssign);
  
  /* pego a string de entrada dos parametros
   * e adiciono um espaço no final para ter 
   * algo para comparar no fim */
  //String input(argv[1] + String(" "));
  String input("a_123abc_ := 123.44 :=876   := ");
  
  std::cout << "using '" << input << "'" << std::endl;
  
  for (int i = 0; i<input.size();i++) {
    table.doTransition(input[i]);
    
    if (table.isInAValidState()) {
      if (table.isInAMatchedState()) {
        std::cout << "Casou '" << table.getMatchedString() << "' como '" << table.getMatchedToken() << "'" << std::endl;
        table.reset();
        /* Volta uma posição na entrada */
        i--;
        continue;
      } else {
      }
    } else {
      std::cout << "Deu pau com estado (" << table.getCurrentState() <<  ") em '" << input[i] << "': " << i << std::endl;
      break;
    }
  }
  
  return 0;
}