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
const String symbols("#!:.=-,;()[]<>&|~+-*/\\.{}%");

/* outros caracteres que podem aparecer em textos corridos */
const String others("@$?");

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
  s,b1,a1,g1,l1,h1,h2,i1,i2,m1,m2,n1,n2,o1,p1,r1,s1,
  s2,t1,t2,u1,w1,f1,k1,x1,c1,c2,c3,d1,d2,e1,J1,j2,q1,q2,yy1,yy2,yy3
  
} LexState;

typedef enum {
  TkId,
  TkComment,
  TkSpaces,

  TkInteger,
  TkLBracket,
  TkRBracket,
  TkLThan,
  TkLEThan,
  TkGThan,
  TkGEThan,
  TkLParentesis,
  TkRParentesis,
  TkLBlock,
  TkRBlock,
  TkAttr,
  TkEqual,
  TkComma,
  TkPlus,
  TkNot,
  TkAnd,
  TkOr,
  TkDiv,
  TkTimes,
  TkDot,
  TkMod,
  TkDiff,
  TkEnd,
  TkString,
  TkStringWord,
  TkCharWord,
  TkMinus,
  TkIs,
  TkIntWord,
  TkBody,
  TkChar,
  TkReturn,
  TkTwoDots,
  TkIf,
  TkElse,
  TkWhile,

  TkRead,
  TkWrite,
  TkWriteLn,

  TkNone,

  TEOF,INVALID
} TokenType;

static map<TokenType,string> TerminalMap {

  {TkId,"TkId"},
  {TkComment,"TkComment"},
  {TkSpaces,"TkSpaces"},
  {TkInteger,"TkInteger"},
  {TkLBracket,"TkLBracket"},
  {TkRBracket,"TkRBracket"},
  {TkLThan,"TkLThan"},
  {TkLEThan,"TkLEThan"},
  {TkGThan,"TkGThan"},
  {TkGEThan,"TkGEThan"},
  {TkLParentesis,"TkLParentesis"},
  {TkRParentesis,"TkRParentesis"},
  {TkLBlock,"TkLBlock"},
  {TkRBlock,"TkRBlock"},
  {TkAttr,"TkAttr"},
  {TkEqual,"TkEqual"},
  {TkComma,"TkComma"},
  {TkPlus,"TkPlus"},
  {TkNot,"TkNot"},
  {TkAnd,"TkAnd"},
  {TkOr,"TkOr"},
  {TkDiv,"TkDiv"},
  {TkTimes,"TkTimes"},
  {TkDot,"TkDot"},
  {TkMod,"TkMod"},
  {TkDiff,"TkDiff"},
  {TkEnd,"TkEnd"},
  {TkString,"TkString"},
  {TkStringWord,"TkStringWord"},
  {TkCharWord,"TkCharWord"},
  {TkMinus,"TkMinus"},
  {TkIs,"TkIs"},
  {TkIntWord,"TkIntWord"},
  {TkBody,"TkBody"},
  {TkChar,"TkChar"},
  {TkReturn,"TkReturn"},
  {TkTwoDots,"TkTwoDots"},
  {TkNone,"TkNone"},
  {TkIf,"TkIf"},
  {TkElse,"TkElse"},
  {TkWhile,"TkWhile"},
  {TkRead,"TkRead"},
  {TkWrite,"TkWrite"},
  {TkWriteLn,"TkWriteLn"},

  {TEOF,"$"},
  {INVALID,"?"}
};

typedef enum {
  EL,
  ArrayTypeName,
  ArrayTypeNameExt,
  ArrayTypeSize,
  AttrStm,
  BasicTypeName,
  Data,
  Expression,
  Funcao,
  Function,
  FunctionBody,
  FunctionCall,
  FunctionContent,
  FunctionImpl,
  FunctionDecl,
  FunctionList,
  FunctionName,
  IfStm,
  IndexAccess,
  IndexAccessExt,
  ListOfFormalParams,
  ListOfParamDef,
  ListOfParamDefExt,
  ParamDef,
  Program,
  Rachel,
  RApid,
  RealParameters,
  ReturnStm,
  Stm,
  StmList,
  TypeName,
  VariableAccess,
  VariableDeclarationList,
  VariableInitialization,
  VariableInitializationListExt,
  WhileStm,
  Set,
  SetContent,
  SetContentExt
} NonTerminal;

static map<NonTerminal,string> NonTerminalMap {
  {EL,"E'"},
  {ArrayTypeName,"ArrayTypeName"},
  {ArrayTypeNameExt,"ArrayTypeNameExt"},
  {ArrayTypeSize,"ArrayTypeSize"},
  {AttrStm,"AttrStm"},
  {BasicTypeName,"BasicTypeName"},
  {Data,"Data"},
  {Expression,"Expression"},
  {Funcao,"Funcao"},
  {Function,"Function"},
  {FunctionBody,"FunctionBody"},
  {FunctionCall,"FunctionCall"},
  {FunctionContent,"FunctionContent"},
  {FunctionImpl,"FunctionImpl"},
  {FunctionDecl,"FunctionDecl"},
  {FunctionList,"FunctionList"},
  {FunctionName,"FunctionName"},
  {IfStm,"IfStm"},
  {IndexAccess,"IndexAccess"},
  {IndexAccessExt,"IndexAccessExt"},
  {ListOfFormalParams,"ListOfFormalParams"},
  {ListOfParamDef,"ListOfParamDef"},
  {ListOfParamDefExt,"ListOfParamDefExt"},
  {ParamDef,"ParamDef"},
  {Program,"Program"},
  {Rachel,"Rachel"},
  {RApid,"RApid"},
  {RealParameters,"RealParameters"},
  {ReturnStm,"ReturnStm"},
  {Stm,"Stm"},
  {StmList,"StmList"},
  {TypeName,"TypeName"},
  {VariableAccess,"VariableAccess"},
  {VariableDeclarationList,"VariableDeclarationList"},
  {VariableInitialization,"VariableInitialization"},
  {VariableInitializationListExt,"VariableInitializationListExt"},
  {WhileStm,"WhileStm"},
  {Set,"Set"},
  {SetContent,"SetContent"},
  {SetContentExt,"SetContentExt"}
};

typedef Grammar<NonTerminal,TokenType> RachelGrammar;

static string symbolToString(const RachelGrammar::Symbol &s)
{
  if (s.isTerminal()) {
    return TerminalMap[s._terminal];
  }
  if (s.isNonTerminal()) {
    return NonTerminalMap[s._nonTerminal];
  }

  if (s.isEmpty()) {
    return "$";
  }
}

RachelGrammar grammar(symbolToString,{
  {EL,{{Program},{TEOF}}},
  {Program,{{Function},{FunctionList}}},
  {FunctionList,{{Function},{FunctionList}}},
  {FunctionList,{}},
  {Function,{{FunctionImpl}}},
  {Function,{{FunctionDecl}}},
  {FunctionImpl,{{TkLParentesis},{TkId},{ListOfFormalParams},{TkTwoDots},{FunctionContent},{TkRParentesis}},{1,2,4}},
  {FunctionImpl,{{TkLParentesis},{TkId},{ListOfFormalParams},{TkIs},{TypeName},{TkTwoDots},{FunctionContent},{TkRParentesis}},{1,2,4,6}},

  {FunctionDecl,{{TkLParentesis},{TkId},{ListOfFormalParams},{TkRParentesis}},{1,2}},
  {FunctionDecl,{{TkLParentesis},{TkId},{ListOfFormalParams},{TkIs},{TypeName},{TkRParentesis}},{1,2,4}},

  {TypeName,{{BasicTypeName}}},
  {TypeName,{{ArrayTypeName}}},

  {BasicTypeName,{{TkIntWord}}},
  {BasicTypeName,{{TkStringWord}}},
  {BasicTypeName,{{TkCharWord}}},

  {ArrayTypeName,{{TkLBracket},{ArrayTypeSize},{TkRBracket},{ArrayTypeNameExt},{BasicTypeName}},{1,3,4}},
  {ArrayTypeNameExt,{{TkLBracket},{ArrayTypeSize},{TkRBracket},{ArrayTypeNameExt}},{1,3}},
  {ArrayTypeNameExt,{}},
  {ArrayTypeSize,{{TkInteger}}},
  {ArrayTypeSize,{}},

  {ListOfFormalParams,{{ListOfParamDef}}},
  {ListOfParamDef,{}},
  {ListOfParamDef,{{ParamDef},{ListOfParamDefExt}}},
  {ListOfParamDefExt,{{TkComma},{ParamDef},{ListOfParamDefExt}},{1,2}},
  {ListOfParamDefExt,{}},

  {ParamDef,{{TypeName},{TkId}}},

  {FunctionContent,{{VariableDeclarationList},{TkBody},{TkTwoDots},{FunctionBody}},{0,3}},

  {VariableDeclarationList,{}},
  {VariableDeclarationList,{{VariableInitialization},{TkEnd},{VariableInitializationListExt}},{0,2}},
  {VariableInitializationListExt,{{VariableInitialization},{TkEnd},{VariableInitializationListExt}},{0,2}},
  {VariableInitializationListExt,{}},
  {VariableInitialization,{{TypeName},{TkId},{TkLParentesis},{Expression},{TkRParentesis}},{0,1,3}},

  {FunctionBody,{{StmList}}},

  {StmList,{{Stm},{TkEnd},{StmList}},{0,2}},
  {StmList,{}},
  {Stm,{{FunctionCall}}},
  {Stm,{{AttrStm}}},
  {Stm,{{IfStm}}},
  {Stm,{{WhileStm}}},
  {Stm,{{ReturnStm}}},

  {FunctionCall,{{TkLParentesis},{FunctionName},{RealParameters},{TkRParentesis}},{1,2}},

  {RealParameters,{{Expression},{RealParameters}}},
  {RealParameters,{}},

  {FunctionName,{{TkId}}},
  {FunctionName,{{TkPlus}}},
  {FunctionName,{{TkMinus}}},
  {FunctionName,{{TkTimes}}},
  {FunctionName,{{TkDiv}}},
  {FunctionName,{{TkMod}}},
  {FunctionName,{{TkAnd}}},
  {FunctionName,{{TkOr}}},
  {FunctionName,{{TkNot}}},
  {FunctionName,{{TkEqual}}},
  {FunctionName,{{TkDiff}}},
  {FunctionName,{{TkGThan}}},
  {FunctionName,{{TkGEThan}}},
  {FunctionName,{{TkLThan}}},
  {FunctionName,{{TkLEThan}}},
  {FunctionName,{{TkRead}}},
  {FunctionName,{{TkWrite}}},
  {FunctionName,{{TkWriteLn}}},

  {AttrStm,{{VariableAccess},{TkAttr},{Expression}},{0,2}},

  {VariableAccess,{{TkId}}},
  {VariableAccess,{{TkId},{IndexAccess}}},
  {IndexAccess,{{TkLBracket},{Expression},{TkRBracket},{IndexAccessExt}},{1,3}},
  {IndexAccessExt,{{IndexAccess}}},
  {IndexAccessExt,{}},

  {IfStm,{{TkIf},{Expression},{TkLBlock},{StmList},{TkRBlock}},{1,3}},
  {IfStm,{{TkIf},{Expression},{TkLBlock},{StmList},{TkRBlock},{TkElse},{TkLBlock},{StmList},{TkRBlock}},{1,3,7}},

  {WhileStm,{{TkWhile},{Expression},{TkLBlock},{StmList},{TkRBlock}},{1,3}},

  {ReturnStm,{{TkReturn},{Expression}},{1}},
  {ReturnStm,{{TkReturn}},{1}},

  {Expression,{{FunctionCall}}},
  {Expression,{{TkInteger}}},
  {Expression,{{TkString}}},
  {Expression,{{TkChar}}},
  {Expression,{{VariableAccess}}},
  {Expression,{{Set}}},

  /* coisas como {2,3,"aa",{2,54,"ee",342}}  */
  {Set,{{TkLBlock},{SetContent},{TkRBlock}},{1}},
  {SetContent,{{Expression},{SetContentExt}}},
  {SetContent,{}},
  {SetContentExt,{{TkComma},{Expression},{SetContentExt}},{1,2}},
  {SetContentExt,{}}
  
},TEOF,INVALID);

TransitionTable<LexState,TokenType> automata(start,invalid,final);

int main(int argc, char **argv)
{
  /* Consome espaços em branco */
  automata.addTransition(start,spaces,s);
  automata.addTransition(s,spaces,s);
  automata.addFinalTransition(s,any - spaces,TkSpaces);
  
  /* Para inteiros e reais */
  automata.addTransition(start,digits,b1);
  automata.addTransition(b1,digits,b1);
  automata.addFinalTransition(b1,any - digits,TkInteger);
  
  /* Para identificador */
  automata.addTransition(start,letters + "_",a1);
  automata.addTransition(a1,letters + digits + "_", a1);
  automata.addFinalTransition(a1,any - letters - digits - "_",TkId);
  
  /* Para [*/
  automata.addTransition(start,"[",g1);
  automata.addFinalTransition(g1,any,TkLBracket);

  /* Para [ */
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
  
  /* Para = e ==*/
  automata.addTransition(start,"=",n1);
  automata.addTransition(n1,"=",n2);
  automata.addFinalTransition(n1,any - "=",TkAttr);
  automata.addFinalTransition(n2,any,TkEqual);
  
  /* Para , */
  automata.addTransition(start,",",o1);
  automata.addFinalTransition(o1,any,TkComma);
  
  /* Para + */
  automata.addTransition(start,"+",p1);
  automata.addFinalTransition(p1,any,TkPlus);
  
  /* Para ~ */
  automata.addTransition(start,"~",r1);
  automata.addFinalTransition(r1,any,TkNot);
  
  /* Para && */
  automata.addTransition(start,"&",s1);
  automata.addTransition(s1,"&",s2);
  automata.addFinalTransition(s2,any,TkAnd);
  
  /* Para || */
  automata.addTransition(start,"|",t1);
  automata.addTransition(t1,"|",t2);
  automata.addFinalTransition(t2,any,TkOr);
  automata.addFinalTransition(t1,any - "|",TkReturn);
  
  /* Para / */
  automata.addTransition(start,"/",u1);
  automata.addFinalTransition(u1,any,TkDiv);
  
  /* Para * */
  automata.addTransition(start,"*",w1);
  automata.addFinalTransition(w1,any,TkTimes);
  
  /* Para . */
  automata.addTransition(start,".",f1);
  automata.addFinalTransition(f1,any, TkDot);
  
  /* separador de comandos ponto e vírgula (;) */
  automata.addTransition(start,";",k1);
  automata.addFinalTransition(k1,any,TkEnd);

  /* Para : */
  automata.addTransition(start,":",x1);
  automata.addFinalTransition(x1,any,TkTwoDots);

  /* char, envonvido por aspas simples, mas só um caractere dentro */
  automata.addTransition(start,"\'",c1);
  automata.addTransition(c1,any - "\'",c2);
  automata.addTransition(c2,"\'",c3);
  automata.addFinalTransition(c3,any,TkChar);

  /* string com aspas duplas */
  automata.addTransition(start,"\"",d1);
  automata.addTransition(d1,any - "\"",d1);
  automata.addTransition(d1,"\"",d2);
  automata.addFinalTransition(d2,any,TkString);
  
  /* Para comentário */
  automata.addTransition(start,"#",e1);
  automata.addTransition(e1,any - breakline,e1);
  automata.addFinalTransition(e1,breakline,TkComment);
  
  /* para símbolo - (minus) */
  automata.addTransition(start,"-",J1);
  automata.addFinalTransition(J1,any - ">",TkMinus);
  
  /* para símbolo -> */
  automata.addTransition(J1,">",j2);
  automata.addFinalTransition(j2,any,TkIs);

  /* para símbolo { */
  automata.addTransition(start,"{",q1);
  automata.addFinalTransition(q1,any,TkLBlock);

  /* para símbolo { */
  automata.addTransition(start,"}",q2);
  automata.addFinalTransition(q2,any,TkRBlock);

  /* Para % */
  automata.addTransition(start,"%",yy1);
  automata.addFinalTransition(yy1,any,TkMod);

  /* Para != */
  automata.addTransition(start,"!",yy2);
  automata.addTransition(yy2,"=",yy3);
  automata.addFinalTransition(yy3,any,TkDiff);

  /* um cara que lê um arquivo do disco */
  FileReader reader(argv[1]);
  
  /* Estrutura com as palavras reservadas */
  TokenHash<TokenType> reservedWords(
    TkNone,{
      {"int",TkIntWord},
      {"integer",TkIntWord},
      {"string",TkStringWord},
      {"body",TkBody},
      {"char",TkCharWord},
      {"if",TkIf},
      {"else",TkElse},
      {"while",TkWhile},

      /* entrada e saída de dados */
      {"read",TkRead},
      {"write",TkWrite},
      {"writeln",TkWriteLn},

      /* aliases para outros símbolos */
      {"and",TkAnd},
      {"or",TkOr},
      {"not",TkNot},
      {"return",TkReturn},
      {"equal",TkReturn},
      {"sum",TkReturn},
      {"sub",TkReturn},
      {"times",TkReturn},
      {"div",TkReturn}
    }
  );
  
  /* Analisador léxico */
  Lexical::Analyser<LexState,TokenType> lexer(reader,automata,reservedWords,TEOF);
  
  // os identificadores devem ser comparados na tabela de palavras reservadas
  lexer.addTokenToCompareWithReserved({TkId});
  
  /* Ignore os seguintes tokens,
   * que não serão passados pro
   * analisador sintático: espaços e comentários
  */
  lexer.ignoreToken({TkSpaces,TkComment});
  
  lexer.setTokenPadding(TkString,1,1);
  lexer.setTokenPadding(TkChar,1,1);

  function<Token<TokenType>()> getToken([&lexer](){
    return lexer.getToken();
  });

  Syntatical::Analyzer<NonTerminal,TokenType> parser(grammar,getToken);

  if (!parser.parse()) {
    cerr << "Erro!" << endl;
    return 1;
  }

  Tree<TokenType,RachelGrammar::Symbol> tree(parser.getTree());

  //cerr << tree.toString<function<string(const RachelGrammar::Symbol &)>>(symbolToString) << endl;

  tree.generateGraph<function<string(const RachelGrammar::Symbol &)>>(symbolToString);

  tree.dispose();

  return 0;
}
