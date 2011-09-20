#include <lexical/transitiontable.h>
#include <common/tokentype.h>
#include <lexical/filereader.h>

#include <lexical/analyser.h>
#include <syntatic/grammar.h>
#include <syntatic/automata.h>

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
  
  TkNone,

  TEOF,INVALID
} TokenType;

static map<TokenType,string> TerminalMap {
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
    
  {TkNone,"TkNone"},

  {TEOF,"$"},

  {INVALID,"?"}
};

typedef enum {
  Program,
  ModuleList,            
  Module,                
  Block,                 
  DefinitionPart,        
  Definition,            
  ConstantDef,           
  VariableDef,           
  Type,                  
  TypeSymbol,            
  TypeSymbolList,            
  Tupletype,             
  Arraytype,             
  Rangetype,             
  VariableList,          
  Typedef,               
  ProcedureDecl,         
  ProcedureDef,          
  ParamPart,             
  ParamDef,              
  ParamDefList,              
  StatementPart,         
  Statement,             
  EmptyStatement,        
  ReadStatement,         
  VariableAccessList,    
  VariableAccessList2,    
  WriteStatement,        
  WriteItem,             
  ExpressionList,        
  ExpressionList2,        
  AssignStatement,       
  IfStatement,           
  GuardedCommandList,    
  GuardedCommandList2,    
  GuardedCommand,        
  DoStatement,           
  ForStatement,          
  ReturnStatement,       
  CallStatement,         
  ArgumentList,          
  Expression,            
  BooleanOperator,       
  RelationalExpression,  
  RelationalOperator,    
  SimpleExpression,      
  SimpleExpressionExt,      
  Term,                  
  TermExt,                  
  Factor,                
  AddingOperator,        
  MultiplyOperator,      
  ConstantName,          
  VariableAccess,        
  VariableMore,          
  Nextitem,              
  Indexorcomp,           
  Constant,              
  BooleanConstant,
  IdList,
  ArrayTypeList,
  WriteItemList,
  BoolRelList,
  EL
} NonTerminal;

static map<NonTerminal,string> NonTerminalMap {
  {Program,"program"},
  {ModuleList,"moduleList"},
  {Module,"module"},                
  {Block,"block"},                 
  {DefinitionPart,"definitionPart"},        
  {Definition,"definition"},            
  {ConstantDef,"constantDef"},           
  {VariableDef,"variableDef"},           
  {Type,"type"},                  
  {TypeSymbol,"typeSymbol"},            
  {TypeSymbolList,"typeSymbolList"},            
  {Tupletype,"tupletype"},             
  {Arraytype,"arraytype"},             
  {Rangetype,"rangetype"},             
  {VariableList,"variableList"},          
  {Typedef,"typedef"},               
  {ProcedureDecl,"procedureDecl"},         
  {ProcedureDef,"procedureDef"},          
  {ParamPart,"paramPart"},             
  {ParamDef,"paramDef"},              
  {ParamDefList,"paramDefList"},              
  {StatementPart,"statementPart"},         
  {Statement,"statement"},             
  {EmptyStatement,"emptyStatement"},        
  {ReadStatement,"readStatement"},         
  {VariableAccessList,"variableAccessList"},    
  {VariableAccessList2,"variableAccessList2"},    
  {WriteStatement,"writeStatement"},        
  {WriteItem,"writeItem"},             
  {ExpressionList,"expressionList"},        
  {ExpressionList2,"expressionList2"},        
  {AssignStatement,"assignStatement"},       
  {IfStatement,"ifStatement"},           
  {GuardedCommandList,"guardedCommandList"},    
  {GuardedCommandList2,"guardedCommandList2"},    
  {GuardedCommand,"guardedCommand"},        
  {DoStatement,"doStatement"},           
  {ForStatement,"forStatement"},          
  {ReturnStatement,"returnStatement"},       
  {CallStatement,"callStatement"},         
  {ArgumentList,"argumentList"},          
  {Expression,"expression"},            
  {BooleanOperator,"booleanOperator"},       
  {RelationalExpression,"relationalExpression"},  
  {RelationalOperator,"relationalOperator"},    
  {SimpleExpression,"simpleExpression"},      
  {SimpleExpressionExt,"simpleExpressionExt"},      
  {Term,"term"},                  
  {TermExt,"termExt"},                  
  {Factor,"factor"},                
  {AddingOperator,"addingOperator"},        
  {MultiplyOperator,"multiplyOperator"},      
  {ConstantName,"constantName"},          
  {VariableAccess,"variableAccess"},        
  {VariableMore,"variableMore"},          
  {Nextitem,"nextitem"},              
  {Indexorcomp,"indexorcomp"},           
  {Constant,"constant"},              
  {BooleanConstant,"booleanConstant"},
  {IdList,"IdList"},
  {ArrayTypeList,"ArrayTypeList"},
  {WriteItemList,"WriteItemList"},
  {BoolRelList,"BoolRelList"},
  {EL,"E'"} 
};

typedef Grammar<NonTerminal,TokenType> GCLGrammar;

static string symbolToString(const GCLGrammar::Symbol &s)
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

/*GCLGrammar grammar(symbolToString,{
  {EL,{{Program},{TEOF}},1},

  // <program>       <module> {<module>}
  {Program,{{ModuleList}},1},

  // <module>        "module" "identifier" <definitionPart> 
  //      [ "private"  <block> ] "."  
  {ModuleList,{{Module},{ModuleList}},1},
  {ModuleList,{},1},
  {Module,{{TkModule},{TkId},{DefinitionPart},{TkDot}},1},
  {Module,{{TkModule},{TkId},{DefinitionPart},{TkPrivate},{Block},{TkDot}},1},

  // <block>        <definitionPart> "begin" <statementPart> "end"
  {Block,{{DefinitionPart},{TkBegin},{StatementPart},{TkEndWord}},1},

  // <definitionPart>   {<definition> ";"}  
  {DefinitionPart,{{Definition},{TkEnd}},1},
  {DefinitionPart,{},1},

  //<definition>      <constantDef> | <variableDef> | <procedureDef> 
  //      | <typedef> |<procedureDecl> 
  {Definition,{{ConstantDef}},1},
  {Definition,{{VariableDef}},1},
  {Definition,{{ProcedureDef}},1},
  {Definition,{{Typedef}},1},
  {Definition,{{ProcedureDecl}},1},

  // <constantDef>       "const" <constantName> "=" <constant> 
  {ConstantDef,{{TkConst},{ConstantName},{TkEqual},{Constant}},1},
  
  // <variableDef>    <type> <variableList>
  {VariableDef,{{Type},{VariableList}},1},
 
  // <type>     <typeSymbol> [ <arraytype> | <rangetype> ] | <tupletype>
  {Type,{{TypeSymbol}},1},
  {Type,{{TypeSymbol},{Arraytype}},1},
  {Type,{{TypeSymbol},{Rangetype}},1},
  {Type,{{Tupletype}},1},

  // <typeSymbol>       "integer" | "Boolean"  | "identifier"
  {TypeSymbol,{{TkInteger},},1},
  {TypeSymbol,{{TkBoolean},},1},
  {TypeSymbol,{{TkReal},},1},
  {TypeSymbol,{{TkString},},1},
  {TypeSymbol,{{TkId},},1},

  // <tupletype>    "[" <typeSymbol> { "," <typeSymbol> } "]"
  {Tupletype,{{TkLBracket},{TypeSymbol},{TypeSymbolList},{TkRBracket}},1},
  {TypeSymbolList,{{TkComma},{TypeSymbol},{TypeSymbolList}},1},
  {TypeSymbolList,{},1},

  // <arraytype>    "array" "[" "identifier" "]" 
  //      {"[" "identifier" "]"}
  {Arraytype,{{TkArray},{TkLBracket},{TkId},{TkRBracket},{ArrayTypeList}},1},
  {ArrayTypeList,{{TkLBracket},{TkId},{TkRBracket},{ArrayTypeList}},1},
  {ArrayTypeList,{},1},

  // <rangetype>    "range" "[" <constant> ".." <constant>  "]"
  {Rangetype,{{TkRange},{TkLBracket},{Constant},{TkTwoDots},{Constant},{TkRBracket}},1},

  // <variableList>   "identifier" {"," "identifier"}
  {VariableList,{{TkId},{IdList}},1},
  {IdList,{{TkComma},{TkId},{IdList}},1},
  {IdList,{},1},

  // <typedef>    "typedef" <type> "identifier" 
  {Typedef,{{TkTypedef},{Type},TkId},1}, 

  // <procedureDecl>  "proc" "identifier" [<paramPart>]
  {ProcedureDecl,{{TkProc},{TkId}},1},
  {ProcedureDecl,{{TkProc},{TkId},{ParamPart}},1},

  // <procedureDef>   <procedureDecl> <block>  
  {ProcedureDef,{{ProcedureDecl},{Block}},1},

  // <paramPart>    "(" [ <paramDef> { ";" <paramDef> } ] )" 
  {ParamPart,{{TkLParentesis},{ParamDef},{ParamDefList},{TkRParentesis}},1},
  {ParamDefList,{{TkEnd},{ParamDef},{ParamDefList}},1},
  {ParamDefList,{},1},

  // <paramDef>     ( "val" | "ref" ) <variableDef> 
  {ParamDef,{{TkVal},{VariableDef}},1},
  {ParamDef,{{TkRef},{VariableDef}},1},

  // <statementPart>  { <statement> ";"} 
  {StatementPart,{{Statement},{TkEnd}},1},

  // <statement>    <emptyStatement> | <readStatement> | <writeStatement> 
  //      | <assignStatement> | <returnStatement> | <callStatement>
  //      | <ifStatement> | <doStatement> | <forStatement>
  {Statement,{{EmptyStatement}},1},
  {Statement,{{ReadStatement}},1},
  {Statement,{{WriteStatement}},1},
  {Statement,{{AssignStatement}},1},
  {Statement,{{ReturnStatement}},1},
  {Statement,{{CallStatement}},1},
  {Statement,{{IfStatement}},1},
  {Statement,{{DoStatement}},1},
  {Statement,{{ForStatement}},1},

  // <emptyStatement>     "skip"
  {EmptyStatement,{{TkSkip}},1},

  // <readStatement>      "read" <variableAccessList>
  {ReadStatement,{{TkRead},{VariableAccessList}},1},

  // <variableAccessList>  <variableAccess> {"," <variableAccess> }
  {VariableAccessList,{{VariableAccess},{VariableAccessList2}},1},
  {VariableAccessList2,{{TkComma},{VariableAccess},{VariableAccessList2}},1},
  {VariableAccessList2,{},1},
  
  // <writeStatement>  "write" <writeItem> {"," <writeItem> } 
  {WriteStatement,{{TkWrite},{WriteItem},{WriteItemList}},1},
  {WriteItemList,{{TkComma},{WriteItem},{WriteItemList}},1},
  {WriteItemList,{},1},

  // <writeItem>       "stringconst" | <expression>  FIXME: is this Ok? Stringconst?
  {WriteItem,{{TkString}},1},
  {WriteItem,{{Expression}},1},
  
  // <expressionList>      <expression> { "," <expression> }  
  {ExpressionList,{{Expression},{ExpressionList2}},1},
  {ExpressionList2,{{TkComma},{Expression},{ExpressionList2}},1},
  {ExpressionList2,{},1},

  // <assignStatement>     <variableAccessList> ":=" <expressionList>  
  {AssignStatement,{{VariableAccessList},{TkAssign},{ExpressionList}},1},

  // <ifStatement>         "if" <guardedCommandList> "fi"  
  {IfStatement,{{TkIf},{GuardedCommandList},{TkFi}},1},  

  // <guardedCommandList>  <guardedCommand> {"[]" <guardedCommand>}  
  {GuardedCommandList,{{GuardedCommand},{GuardedCommandList2}},1},
  {GuardedCommandList2,{{TkGuarded},{GuardedCommand},{GuardedCommandList2}},1},
  {GuardedCommandList2,{},1},

  // <guardedCommand>  <expression> "->" <statementPart>  
  {GuardedCommand,{{Expression},{TkThen},{StatementPart}},1},

  // <doStatement>         "do" <guardedCommandList> "od" 
  {DoStatement,{{TkDo},{GuardedCommandList},{TkOd}},1},  

  // <forStatement>    "forall" <variableAccess> "->" <statementPart> "llarof"
  {ForStatement,{{TkForall},{VariableAccess},{TkThen},{StatementPart},{TkLlarof}},1},
  
  // <returnStatement> "return" <expression>
  {ReturnStatement,{TkReturn},1},

  // <callStatement>   "identifier" ["." "identifier"] <argumentList>
  {CallStatement,{{TkId},{TkDot},{TkId},{ArgumentList}},1},
  {CallStatement,{{TkId},{ArgumentList}},1},
  
  // <argumentList>    "(" [ <expressionList> ] ")"
  {ArgumentList,{{TkLParentesis},{ExpressionList},{TkRParentesis}},1},
  {ArgumentList,{{TkLParentesis},{TkRParentesis}},1},
 
  // <expression>    <relationalExpression> {<booleanOperator> <relationalExpression>}
  {Expression,{{RelationalExpression},{BoolRelList}},1},
  {BoolRelList,{{BooleanOperator},{RelationalExpression}},1},
  {BoolRelList,{},1},

  // <booleanOperator> "&" | "|" 
  {BooleanOperator,{{TkAnd}},1},
  {BooleanOperator,{{TkOr}},1},
    
  // <relationalExpression>  <simpleExpression> [<relationalOperator> <simpleExpression>] 
  {RelationalExpression,{{SimpleExpression}},1},
  {RelationalExpression,{{SimpleExpression},{RelationalOperator},{SimpleExpression}},1},
  
  // <relationalOperator>  "<" | "=" | ">" | "<=" | ">=" | "#" 
  {RelationalOperator,{{TkLThan}},1},
  {RelationalOperator,{{TkEqual}},1},
  {RelationalOperator,{{TkGThan}},1},
  {RelationalOperator,{{TkLEThan}},1},
  {RelationalOperator,{{TkGEThan}},1},
  {RelationalOperator,{{TkSharp}},1},

  // <simpleExpression>  ( "+" | "-" )  <term> { <addingOperator> <term>} | <term> { <addingOperator> <term>}  
  {SimpleExpression,{{SimpleExpression},{TkPlus},{Term},{SimpleExpressionExt}},1}, 
  {SimpleExpression,{{SimpleExpression},{TkMinus},{Term},{SimpleExpressionExt}},1}, 
  {SimpleExpressionExt,{{AddingOperator},{Term}},1},
  {SimpleExpressionExt,{{Term},{AddingOperator}},1},
  {SimpleExpressionExt,{},1},

  // <term>      <factor> {<multiplyOperator> <factor>}  
  {Term,{{Factor},{TermExt}},1},
  {TermExt,{{MultiplyOperator},{Factor}},1},
  {TermExt,{},1},
  
  // <factor>    <variableAccess> | "number" | <booleanConstant> 
  //        | "[" <expressionList> "]" 
  //        | "(" <expression> ")" | "~" <factor> 
  {Factor,{{VariableAccess}},1},
  {Factor,{{TkInteger}},1},
  {Factor,{{BooleanConstant}},1},
  {Factor,{{TkLBracket},{ExpressionList},{TkRBracket}},1},
  {Factor,{{TkLParentesis},{Expression},{TkRParentesis}},1},
  {Factor,{{TkNot},{Factor}},1},

  // <addingOperator>  "+" | "-"  
  {AddingOperator,{{TkPlus}},1},
  {AddingOperator,{{TkMinus}},1},

  // <multiplyOperator>    "*" | "/" | "\" 
  {MultiplyOperator,{{TkPlus},{TkDiv},{TkRem}},1},

  // <constantName>        "identifier"  
  {ConstantName,{{TkId}},1},

  // <variableAccess>      "identifier" <variableMore>  
  {VariableAccess,{{TkId},{VariableMore}},1},

  // <variableMore>        ""  |  "[" <expression> "]"  <indexorcomp>
  //          | "." <nextitem>  <indexorcomp>
  {VariableMore,{},1},  
  {VariableMore,{{TkLBracket},{Expression},{TkRBracket},{Indexorcomp}},1},
  {VariableMore,{{TkDot},{Nextitem},{Indexorcomp}},1},

  // <nextitem>      "number" | "identifier" // XXX: adicionei string e real
  {Nextitem,{{TkInteger}},1},
  {Nextitem,{{TkReal}},1},
  {Nextitem,{{TkId}},1},
  {Nextitem,{{TkString}},1},

  // <indexorcomp>   { "."  "number" | "[" <expression> "]" } // TODO: este number pode ser uma variável?
  {Indexorcomp,{},1},
  {Indexorcomp,{{TkDot},{TkInteger}},1},
  {Indexorcomp,{{TkLBracket},{Expression},{TkRBracket}},1},
  
  // <constant>      <expression>
  {Constant,{{Expression}},1},

  // <booleanConstant>     "true" | "false" 
  {BooleanConstant,{{TkTrue}},1},
  {BooleanConstant,{{TkFalse}},1}
  
},TEOF,INVALID);*/

TransitionTable<LexState,TokenType> automata(start,invalid,final);

GCLGrammar ifG(symbolToString,{
  {EL,{{Program},{TEOF}}},
  {Program,{{Module},{TkId}}},
  {Module,{{TkIf},{TkLParentesis},{Block},{TkRParentesis},{Definition}}},
  {Module,{{TkIf},{TkLParentesis},{Block},{TkRParentesis},{Definition},{TkGuarded},{Definition}}},
  {Block,{{TkString}}},
  {Block,{{BooleanConstant}}},
  {Module,{{TkId}}},
  {BooleanConstant,{{TkTrue}}},
  {BooleanConstant,{{TkFalse}}},
  {Definition,{{Module}}}
},TEOF,INVALID);

int main(int argc, char **argv)
{
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
    }
  );
  
  /* Analisador léxico */
  Analyser<LexState,TokenType> lexer(reader,automata,reservedWords,TEOF);
  
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

  function<Token<TokenType>()> getToken([&lexer](){
    return lexer.getToken();
  });

  Automata<NonTerminal,TokenType> syntaticAutomata(ifG,getToken);

  ifG.printTable();

  if (!syntaticAutomata.parse()) {
    cerr << "Erro!" << endl;
  }

  Tree<TokenType,GCLGrammar::Symbol> tree(syntaticAutomata.getTree());

  cerr << tree.toString<function<string(const GCLGrammar::Symbol &)>>(symbolToString) << endl;

  //ifG.generateGraph();
  
  
  return 0;
}
