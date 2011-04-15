#include <iostream>
#include <lexical/transitiontable.h>
#include <common/string.h>
#include <common/tokentype.h>

using namespace Common;
using namespace Lexical;

typedef String State;
typedef String Token;

State s = "start", a1 = "a1", a2 = "a2", a3 = "a3", a4 = "a4", invalid = "invalid", final = "final", s1 = "s1", s2 = "s2";
Token comment = "comment", space = "space", TKif = "if", TKinvalid = "invalido", TKfor = "for";

int main(int argc, char** argv) {
  /*TokenHash<Token> Hash(TKinvalid);
  Hash.add("if", TKif);
  Hash.add("for", TKfor);
  std::cout << Hash.findReservedWord("For") << "\n"; */
  
  TransitionTable<State, Token> table(s, invalid, final, TKinvalid);
  table.addTransition(s, "/", a1);
  table.addTransition(a1, "*", a2);
  table.addTransition(a2, " aeiou", a2);
  table.addTransition(a2, "*", a3);
  table.addTransition(a3, "/", a4);
  table.addTransition(a3, " aeiou", a2);
  table.addFinalTransition(a4, " aeiou", comment);
  table.addTransition(s, " ", s1);
  table.addTransition(s1, " ", s1);
  table.addFinalTransition(s1, "/*aeiou", space);
  
  String input(argv[1] + String(" "));
  
  for (int i=0; i<input.size(); i++) {
    table.doTransition(input[i]);
    if (table.isInAValidState()){
      if (table.isInAMatchedState()) {
        std::cout << table.getMatchedString() << " " << table.getMatchedToken() << "\n";
        table.reset();
        i--;
      }
    } else {
      std::cout << "Travou em " << i << " caracter: " << input[i] << "\n";
      break;
    }
      
  }
  
  return 0;
}

