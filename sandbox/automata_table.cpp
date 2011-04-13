#include <src/lexical/transitiontable.h>
#include <src/lexical/tokentype.h>

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
  final_assign,
  
  /* Final genérico */
  final
} State;

int main(int argc, char **argv)
{
  using namespace Lexical;
  
  const String digits("0123456789");
  const String letters("abcdefghijklmnopqrstuvxwyzABCDEFGHIJKLMNOPQRSTUVXWYZ");
  const String symbols(".=-,;()[]#<>&|~+-*/\\.");
  
  /* aspas (quotes) e apóstrofos (apostrophes) */
  const String quotes("\'\"");
  
  const String blanks("\t ");
  const String breakline("\n");
  const String separators(symbols + blanks);
  
  /* Estrutura com as palavras reservadas */
  TokenHash reservedHash;
  reservedHash["if"] = If;
  
  TransitionTable<State,TokenType> table(start,invalid,final);
  
  /* Para inteiros e reais */
  table.addTransition(start,digits,b1);
  table.addTransition(b1,".",b2);
  table.addTransition(b1,digits,b1);
  table.addTransition(b2,digits,b3);
  table.addTransition(b3,digits,b3);
  table.addTransition(b3,separators + quotes,final_real);
  table.addTransition(b1,separators + quotes,final_number);
  
  table.addMatched(final_number,None);
  table.addMatched(final_real,None);
  
  /* Para identificador */
  table.addTransition(start,letters,a1);
  table.addTransition(a1,letters + digits, a1);
  table.addTransition(a1,"_",a2);
  table.addTransition(a2,letters + digits, a1);
  table.addTransition(a1,separators + quotes,final_id);
  table.addTransition(a2,separators + quotes,final_id);
  
  table.addMatched(final_id,None);
  
  /* string com aspas simples*/
  table.addTransition(start,"\'",c1);
  table.addTransition(c1,letters + digits + blanks,c1);
  table.addTransition(c1,"\'",c3);
  table.addTransition(c3,separators + digits + letters,final_string);
  
  /* string com aspas duplas */
  table.addTransition(start,"\"",c2);
  table.addTransition(c2,letters + digits + blanks,c2);
  table.addTransition(c2,"\"",c3);
  
  table.addMatched(final_string,None);
  
  /* Para atribuição */
  table.addTransition(start,":",e1);
  table.addTransition(e1,"=",e2);
  table.addTransition(e2,digits + letters + separators + quotes,final_assign);
  
  table.addMatched(final_assign,None);
  
  String input(argv[1] + String(" "));
  String match;
  
  State s(start);
  
  for (int i = 0; i<input.size();i++) {
    s = table.doTransition(s,input[i]);
    
    if (table.hasStateAsValid(s)) {
      if (table.isMatchedState(s)) {
        std::cout << "Casou " << match << std::endl;
        table.reset(s,match,i);
        continue;
      } else {
        //std::cout << "Em " << input[i] << ": " << i << std::endl;
      }
      
      match += input[i];
    } else {
      std::cout << "Deu pau em (" << (int)s <<  ")" << input[i] << ": " << i << std::endl;
      break;
    }
  }
  
  return 0;
}