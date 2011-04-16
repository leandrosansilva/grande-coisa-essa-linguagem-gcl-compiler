#include <iostream>
#include <lexical/transitiontable.h>
#include <common/string.h>
#include <common/tokentype.h>
#include <lexical/analyser.h>
#include <lexical/filereader.h>

/* Para usar string como tipo */
//typedef Common::String State;
//typedef Common::String Token;
//State s = "start", a1 = "a1", a2 = "a2", a3 = "a3", a4 = "a4", invalid = "invalid", final = "final", s1 = "s1", s2 = "s2";
//Token comment = "comment", space = "space", TKif = "if", TKinvalid = "invalido", TKfor = "for";

/* Para usar enum... */
typedef enum {s, a1, a2, a3, a4, invalid, final, s1, s2 } State;
typedef enum {comment, space, TKif, TKinvalid, TKfor} Token;

int main(int argc, char** argv) {
  Lexical::TokenHash<Token> Hash(TKinvalid);
  
  Lexical::TransitionTable<State, Token> table(s, invalid, final);
  table.addTransition(s, "/", a1);
  table.addTransition(a1, "*", a2);
  table.addTransition(a2, " \naeiou", a2);
  table.addTransition(a2, "*", a3);
  table.addTransition(a3, "/", a4);
  table.addTransition(a3, " \naeiou", a2);
  table.addFinalTransition(a4, " \naeiou", comment);
  table.addTransition(s, " \n", s1);
  table.addTransition(s1, " \n", s1);
  table.addFinalTransition(s1, "/*aeiou", space);
  
  Lexical::FileReader file(argv[1]);
  
  Lexical::Analyser<State,Token> analyser(file,table,Hash);
  
  analyser.ignoreToken(space);
  
  Common::Token<Token> token;
  
  while (analyser.canReadToken()) {
    token = analyser.getToken();
    
    std::cout << "Pegou '" << token.getLexema() << "' do tipo '" << token.getType() << "'" << std::endl;
  }
  
  return 0;
}

