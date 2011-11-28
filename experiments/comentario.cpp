#include <iostream>
#include <lexical/transitiontable.h>
#include <common/string.h>
#include <common/tokentype.h>
#include <lexical/analyser.h>
#include <lexical/filereader.h>

typedef enum {s, a1, a2, a3, a4, invalid, final, s1, s2 } State;

typedef enum {comment, space, TKif, TKinvalid, TKfor, TEOF} Token;

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
  
  Lexical::Analyser<State,Token> analyser(file,table,Hash, TEOF);
  
  analyser.ignoreToken({space});

  analyser.setTokenPadding(comment,2,2);
  
  while (analyser.canReadToken()) {
    const Common::Token<Token> token(analyser.getToken());
    std::cout << "Pegou '" << token.getLexema() << "' do tipo '" << token.getType() << "'" << std::endl;
  }
  
  return 0;
}

