#include <lexical/transitiontable.h>
#include <common/tokentype.h>
#include <lexical/filereader.h>

#include <lexical/analyser.h>
#include <syntatic/grammar.h>
#include <syntatic/analyser.h>
#include <semantic/analyser.h>

#include <stack>
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
  RealParameters,
  ReturnStm,
  Stm,
  StmList,
  StmListExt,
  TypeName,
  VariableAccess,
  VariableDeclarationList,
  VariableInitialization,
  VariableInitializationListExt,
  WhileStm,
  Set,
  SetContent,
  SetContentExt,
  Literal,
  NonLiteral,
  LiteralSet,
  LiteralSetContent,
  LiteralSetContentExt

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
  {RealParameters,"RealParameters"},
  {ReturnStm,"ReturnStm"},
  {Stm,"Stm"},
  {StmList,"StmList"},
  {StmListExt,"StmListExt"},
  {TypeName,"TypeName"},
  {VariableAccess,"VariableAccess"},
  {VariableDeclarationList,"VariableDeclarationList"},
  {VariableInitialization,"VariableInitialization"},
  {VariableInitializationListExt,"VariableInitializationListExt"},
  {WhileStm,"WhileStm"},
  {Set,"Set"},
  {SetContent,"SetContent"},
  {SetContentExt,"SetContentExt"},
  {LiteralSet,"LiteralSet"},
  {Literal,"Literal"},
  {NonLiteral,"NonLiteral"},
  {LiteralSetContent,"LiteralSetContent"},
  {LiteralSetContentExt,"LiteralSetContentExt"}
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

  {FunctionContent,{{VariableDeclarationList},{StmList}}},

  {VariableDeclarationList,{}},
  {VariableDeclarationList,{{VariableInitialization},{VariableInitializationListExt},{TkEnd}},{0,1}},
  {VariableInitializationListExt,{{TkComma},{VariableInitialization},{VariableInitializationListExt}},{1,2}},
  {VariableInitializationListExt,{}},
  {VariableInitialization,{{TypeName},{TkId},{TkLParentesis},{Expression},{TkRParentesis}},{0,1,3}},

  // a última instrução não pode terminar com ponto-e-vírgula
  {StmList,{}},
  {StmList,{{Stm},{StmListExt}}},
  {StmListExt,{{TkEnd},{Stm},{StmListExt}},{1,2}},
  {StmListExt,{}},

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

  {Expression,{{Literal}}},
  {Expression,{{NonLiteral}}},

  /* NonLiteral é um valor calculado */
  {NonLiteral,{{FunctionCall}}},
  {NonLiteral,{{VariableAccess}}},
 
  /* literal é um valor constante inserido no código */
  {Literal,{{TkInteger}}},
  {Literal,{{TkString}}},
  {Literal,{{TkChar}}},
  {Literal,{{LiteralSet}}},

  // literalset é algo como {1,2,3,"lala",'a',{3,4,78},5}
  {LiteralSet,{{TkLBlock},{LiteralSetContent},{TkRBlock}},{1}},
  {LiteralSetContent,{{Literal},{LiteralSetContentExt}}},
  {LiteralSetContent,{}},
  {LiteralSetContentExt,{{TkComma},{Literal},{LiteralSetContentExt}},{1,2}},
  {LiteralSetContentExt,{}}

},TEOF,INVALID);

typedef Semantic::Analyser<NonTerminal,TokenType,string> RachelSemanticAnalyser;
typedef Tree<TokenType,RachelGrammar::Symbol> RachelTree;

// Uma tabela de variáveis. nome -> tipo 
typedef map<string,string> VariableTable;

// mapa string -> tupla com um nome de tipo seguido de uma lista de tipos 
// (casa parametros por posição)
typedef map<string,tuple<string,list<string>>> FunctionTable;

// um mapa de tipos da linguagem para tipos do llvm
typedef map<string,string> TypeTable;

// Quais valores estão em registradores?
set<string> variablesInRegister;

// qual a função em que estou?
string currentFunction;

// identifica a variável que estou manipulando num determinado nó
stack<string> varStack;

TypeTable typeTable {
  {"int","i32"},
  {"char","i8"},
  {"void","void"}
};

// mapeia um tipo para seu alinhamento
map<string,int> typeAlign {
  {"i32",4},
  {"i8",1}
};

// Uma tabela com todas as funções declaradas
FunctionTable functionTable;

// Uma lista de variáveis, referindo-se a função corrente
VariableTable variableTable;

// obtem um novo nome para um temporário
class Counter
{
protected:
  int _counter;
  function<string(int)> _callBack;
public:
  Counter(function<string(int)> callBack):
  _callBack(callBack),
  _counter(0)
  {
  }
  
  void reset()
  {
    _counter = 0;
  }
  
  string operator()()
  {
    return _callBack(_counter++);
  }
};

Counter createNewTempName([](int counter) -> string {
  stringstream r;
  r << counter;
  return r.str();
});

Counter createNewLabelName([](int counter) -> string {
  stringstream r;
  r << "label" << counter;
  return r.str();
});

struct ProgramAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));
    string s1(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));
    return s0 + s1;
  }
};

struct FunctionListAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    // se houver uma "cauda", ou seja, mais funções à frente, processa
    if (t.size()) {
      string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));
      string s1(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));
      return s0 + s1;
    }
    return "";
  }
};

struct FunctionAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    return getAnalyser(t.getChild(0))->getCode(t.getChild(0));
  }
};

struct ArrayTypeNameAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    throw string("tipo array não implementado!");
  }
};

struct BasicTypeNameAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    string typeName(t.getChild(0).getToken().getLexema());
  
    if (typeName == "string") {
      throw string("tipo string não implementado");
    }

    return typeTable[typeName];
  }
};

struct TypeNameAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    return getAnalyser(t.getChild(0))->getCode(t.getChild(0));
  }
};

struct FunctionImplAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    // zero as tabela
    variableTable.clear();
    variablesInRegister.clear();
    while (varStack.size()) varStack.pop();
    createNewTempName.reset();
    
    string functionName(t.getChild(0).getToken().getLexema());
    
    auto found(functionTable.find(functionName));

    // se achou, verifica se já está definida
    if (found != functionTable.end()) {
      throw string("função " + functionName + " já está definida");
    }

    // olho se o terceiro nó é o de tipo ou o próprio corpo da função
    int contentNode((t.symbol(2) == FunctionContent) ? 2 : 3);

    // por omissão de tipo, uma função é void
    string functionType(typeTable["void"]);

    // se o corpo está no nó 3, o nó 2 possui o tipo de retorno da função 
    if (contentNode == 3) {
      functionType = getAnalyser(t.getChild(2))->getCode(t.getChild(2));
    }
    
    // informo a função que estou analisando
    currentFunction = functionName;

    // FIXME: que estrutura usar para a assinatura da função? 
    // uma tupla com um tipo de retorno e uma lista de tipos
    functionTable[functionName] = {};
    get<0>(functionTable[currentFunction]) = functionType;
    
    return "define " + functionType + " @" + functionName + // nome da função
           "(" + getAnalyser(t.getChild(1))->getCode(t.getChild(1)) + ") {\n" +  // parametros
            getAnalyser(t.getChild(contentNode))->getCode(t.getChild(contentNode)) + // corpo
            "}\n\n"; // fechamento da função
  }
};

struct ListOfFormalParamsAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    return (t.getChild(0).size() == 0) 
      ? ""
      : getAnalyser(t.getChild(0))->getCode(t.getChild(0));
  }
};

struct ListOfParamDefAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0))); 
    string s1(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));
    return s0 + s1;
  }
};

struct ListOfParamDefExtAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    if(t.size()) {
      string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));
      string s1(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));
      return ", " + s0 + s1;
    } else return "";
  }
};

struct ParamDefAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    // o nome da variável
    string varName(t.getChild(1).getToken().getLexema());

    // o tipo da variável
    string typeName(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));

    if (variableTable.find(varName) != variableTable.end()) {
      throw string("variável " + varName + " já foi definida!");
    }

    // coloco a variável na tabela de variável
    variableTable[varName] = typeName;
    
    // insiro o tipo na assinatura da função
    get<1>(functionTable[currentFunction]).push_front(typeName);

    return typeName + " %" + varName;
  }
};

struct FunctionCallAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  int countParameters(RachelTree &t)
  {
    // obtem a quantidade de parâmetros da função
    // deve ser uma cópia de t, não uma referência
    RachelTree paramTree(t);
    int c(0);
    while (paramTree.size()) {
      c++;
      paramTree = paramTree.getChild(1);
    }
    
    return c;
  }
  
  string getCode(RachelTree &t)
  {
    // no caso seja uma função definida pelo usuário
    if (t.getChild(0).getChild(0).getHead() == TkId) {
      
      string functionName(t.getChild(0).getChild(0).getToken().getLexema());
      auto found(functionTable.find(functionName));
      
      // se não achou a função, é erro
      if (found == functionTable.end()) {
        throw "função desconhecida: " + functionName;
      }
     
      // quantos parâmetros foram passados para a função?
      int paramsC(countParameters(t.getChild(1)));
      
      // checa se a quantidade de parâmetros na chamada é a mesma que a da declaração
      // da função
      if (get<1>(found->second).size() != paramsC) {
        stringstream r;
        r << "número de parametros passados em " << functionName << ": " << paramsC  << " contra " 
          << get<1>(found->second).size() << " declarados";
        throw r.str();
      }
      
      // tendo certeza que o número de parâmetros está ok, 
      // posso gerar o código dos parâmetros passados
      string s0(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));
      
      // chegando neste ponto já devo ter na pilha os parâmetros que vou usar
      list<tuple<string,string>> params;
      
      stringstream r;
      
      // retiro-os da pilha, um a um, o jogo num novo temporário (instrução load)
      // e uso este temporário para a operação
      // %3 = load i32* %1, align 4
      for (int i(0); i < paramsC; i++) {
        string varName,typeName;
       
        // se o valor está em registrador, não preciso carregá-lo da memória
        if (variablesInRegister.find(varStack.top()) != variablesInRegister.end()) {
          varName = varStack.top();
        } else {
          // caso contrário, preciso criar um novo temporário com o que vem da memória
          varName = createNewTempName();
          // digo que se trata de um valor em registrador
          variablesInRegister.insert(varName);
          
          r << "  %" << varName << " = " << "load " << variableTable[varStack.top()]
            << "* " << "%" << varStack.top() << ", " << "align " << typeAlign[variableTable[varStack.top()]] << "\n";         
        }
        
        typeName = variableTable[varStack.top()];
        
        // variavel -> tipo
        params.push_front(make_tuple(varName,typeName));
        
        varStack.pop();
      }

      // empilha o temporário que guarda o resultado da função
      string left(createNewTempName());
      
      varStack.push(left);
      
      variableTable[left] = get<0>(found->second);
      
      // digo que se trata de um valor em registrador
      variablesInRegister.insert(left);
      
      // FIXME: algumas funções são void e não retornam coisa alguma.
      // tratar este caso e fazer a verificação dos tipos de retorno
      r << "  %" << left << " = call " << get<0>(found->second) << " @" << functionName << "(";
     
      // emito o código dos parâmetros
      for (auto i(params.begin()); i!= params.end(); i++) {
        r << get<1>(*i) << " %" << get<0>(*i);
        
        if (distance(params.begin(), i) < params.size() - 1) {
          r << ", ";
        }
      }
      
      r << ")\n";
      
      // retorna a criação do código das chamadas de função mais a chamada em si
      return s0 + r.str();
    }
    return "";
  }
};

struct RealParametersAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree& t)
  {
    // se é o último parâmetro, não há nada
    if (t.size() == 0) {
      return "";
    }
    
    // gero o código do parâmetro
    string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));
    
    // neste momento tenho no topo da pilha o temporário 
    // que guarda o valor do parâmetro
     
    
    string s1(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));
    
    return s0 + s1;
  }
};

struct ExpressionAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    return getAnalyser(t.getChild(0))->getCode(t.getChild(0));
  }
};

struct LiteralAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    // faz as checagens de coisas ainda não implementadas
    if (t.getChild(0).getHead() == LiteralSet) {
      throw string("literalset não implementado");
    }
    
    if (t.getChild(0).getHead() == TkString) {
      throw string("string não implementada");
    }
    
    if (t.getChild(0).getHead() == TkChar) {
      throw string("char não implementada :-(");
    }
    
    string value, type;
    
    if (t.getChild(0).getHead() == TkInteger) {
      value = t.getChild(0).getToken().getLexema();
      type = typeTable["int"];
    }
    
    stringstream r;

    string tempName(createNewTempName());
    
    r << "  %" << tempName << " = " << "alloca i32, align 4" << "\n";
    r << "  store i32 " << value << ", i32* %" << tempName << ", align 4" << "\n";
    
    string tempName2(createNewTempName());
    
    r << "  %" << tempName2 << " = load i32* %" << tempName << ", align 4" << "\n";
    
    varStack.push(tempName2);
    
    variablesInRegister.insert(tempName2);
    
    variableTable[tempName2] = type;
    
    return r.str();
  }
};

struct NonLiteralAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    return getAnalyser(t.getChild(0))->getCode(t.getChild(0));
  }
};

struct VariableAccessAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    // se estiver acessando um índice de array, 
    // é erro, pois ainda não foi implementado
    if (t.size() == 2) {
      throw string("Acesso em array ainda não implementado");
    }
    
    // se chegou aqui, devo buscar a variável da memória 
    // e colocar num temporário
    string varName(t.getChild(0).getToken().getLexema());
    
    // se não achou a variável, é erro
    if (variableTable.find(varName) == variableTable.end()) {
      throw string("variável " + varName + " não declarada!");
    }
    
    // variável encontrada
    // Insiro-la na pilha de variáveis
    varStack.push(varName);
    
    // não há o que retornar
    return "";
  }
};

struct FunctionContentAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    // Aloca a variável de retorno
    string sEntry("entry:\n");
    string sp("  %__retval__ = alloca i32, align 4\n");
    
    string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));
    string s1(getAnalyser(t.getChild(1))->getCode(t.getChild(1))); 
    
    // define o final da função, com o último bloco
    string returnTemp(createNewTempName());
    
    string sr(
      // FIXME: nem sempre! 
      //"  ret i32 42\n"
      
      // e a label de saída da função
      "return:\n"
      "  %" + returnTemp + " = load i32* %__retval__\n"
      "  ret i32 %" + returnTemp + "\n"
    );
    
    return sEntry + sp + s0 + s1 + sr;
  }
};

struct VariableDeclarationListAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    if (t.size()) {
      string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));
      string s1(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));
      return s0 + s1;
    }
    else return "";
  }
};

struct VariableInitializationAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    string varName(t.getChild(1).getToken().getLexema());
    string typeName(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));

    // a variável já foi declarada?
    if (variableTable.find(varName) != variableTable.end()) {
      throw string("variável " + varName + " já está definida!");
    }

    // a cara é %<var> = alloca <tipo>, align <size>
    stringstream c;
    c << "  %" << varName
      << " = alloca " << typeName 
      << ", align " << typeAlign[typeName]
      << "\n";

    // insiro a variável na tabela de símbolos
    variableTable[varName] = typeName;
    
    // retorna alocação + código de inicialização da variável e coloca um 
    // temporário na pilha
    string r(c.str() + getAnalyser(t.getChild(2))->getCode(t.getChild(2)));
    
    stringstream f;
    f << "  store " << typeName << " %" << varStack.top() << ", " << typeName 
      << "* " << "%" << varName << ", " << "align " << typeAlign[typeName] 
      << "\n";
    
    // tiro a variável do topo da pilha
    varStack.pop();
    
    return r + f.str();
  }
};

struct VariableInitializationListExtAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    if (t.size()) {
      string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));
      string s1(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));
      return s0 + s1;
    } else {
      return "";
    }
  }
};

struct StmListAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    if (!t.size()) { 
      // FIXME: tamanho 0, só pode passar caso a função seja void
      return "";
    }
    
    string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));
    string s1(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));

    return s0 + s1;
  }
};

struct StmAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    return getAnalyser(t.getChild(0))->getCode(t.getChild(0));
  }
};

struct ReturnStmAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    // pega o código gerado pela expressão e coloca na pilha
    string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));
    
    // a variável do topo da pilha está em registrador? 
    // Se não, preciso buscá-la da memória
    string topVar(varStack.top());
    string tempVar(topVar);
    varStack.pop();
    
    string s1;
    
    if (variablesInRegister.find(topVar) == variablesInRegister.end()) {
      tempVar = createNewTempName();
      s1 = "  %" + tempVar + " = load i32* %" + topVar + ", align 4" + "\n";
    }
    
    // coloca o valor na variável de retorno e pula para a label de saída
    string s2(
      "  store i32 %" + tempVar + ", i32* %__retval__\n"
      "  br label %return\n\n"
    );
    
    return s0 + s1 + s2;
  }
};

struct IfStmAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    // pega o código gerado pela expressão e coloca na pilha 
    string s0(getAnalyser(t.getChild(0))->getCode(t.getChild(0)));
    
    // TODO: há dois tipos diferentes de if: com e sem else
    
    string cmpVar(createNewTempName());
    
    // possui o bloco else?
    bool hasElse(t.size() == 3);
    
    string thenLabel(createNewLabelName());
    string elseLabel;
    // se possui else, cria uma nova label
    if (hasElse) elseLabel = createNewLabelName();
    
    string ifEndLabel(createNewLabelName());
    
    stringstream r;
    r << "  %" << cmpVar << " = icmp ne i32 %" << varStack.top() << ", 0\n";
    r << "  br i1 %" << cmpVar << ", label %" << thenLabel;
    r << ", label %" << (hasElse ? elseLabel : ifEndLabel) << "\n";
    
    // pego o código gerado pelo bloco then
    string thenCode(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));
    
    r << thenLabel << ":\n";
    r << thenCode;
    r << "  br label %" << ifEndLabel << "\n";
    
    // se possui else gero seu código
    if (hasElse) {
      string elseCode(getAnalyser(t.getChild(2))->getCode(t.getChild(2)));
      r << elseLabel << ":\n";
      r << elseCode;
      r << "  br label %" << ifEndLabel << "\n";
    }
    
    r << ifEndLabel << ":\n";

    varStack.pop();
    
    return s0 + r.str();
  }
};

struct AttrStmAnalyser: public RachelSemanticAnalyser::NodeAnalyser
{
  string getCode(RachelTree &t)
  {
    // Primeiro coloco no topo da pilha o resultado da expressão da direita
    string s0(getAnalyser(t.getChild(1))->getCode(t.getChild(1)));
    
    // pego o resultado da expressão
    string result(varStack.top());
    varStack.pop();
    
    // coloco o nome da variável na pilha
    getAnalyser(t.getChild(0))->getCode(t.getChild(0));
    
    string s1("  store i32 %" + result + ", i32* %" + varStack.top() + "\n");
    
    varStack.pop();
    
    return s0 + s1;
  }
};

int main(int argc, char **argv)
{
  TransitionTable<LexState,TokenType> automata(start,invalid,final);

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
  
  /* Para [ */
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
  
  /* Para = e == */
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
  
  /* Para || e | */
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

  /* para símbolo } */
  automata.addTransition(start,"}",q2);
  automata.addFinalTransition(q2,any,TkRBlock);

  /* Para % */
  automata.addTransition(start,"%",yy1);
  automata.addFinalTransition(yy1,any,TkMod);

  /* Para != */
  automata.addTransition(start,"!",yy2);
  automata.addTransition(yy2,"=",yy3);
  automata.addFinalTransition(yy3,any,TkDiff);

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
      {"mul",TkReturn},
      {"div",TkReturn}
    }
  );

  /* um cara que lê um arquivo do disco */
  FileReader reader(argv[1]);

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

  Syntatical::Analyser<NonTerminal,TokenType> parser(grammar,[&lexer](){
    return lexer.getToken();
  });

  if (!parser.parse()) {
    cerr << "Erro!" << endl;
    return 1;
  }

  RachelTree tree(parser.getTree());

  //cerr << tree.toString<function<string(const RachelGrammar::Symbol &)>>(symbolToString) << endl;
  //tree.generateGraph<function<string(const RachelGrammar::Symbol &)>>(symbolToString);
  
  RachelSemanticAnalyser a(tree,{
    {Program,new ProgramAnalyser},
    {FunctionList,new FunctionListAnalyser},
    {Function,new FunctionAnalyser},
    {FunctionImpl,new FunctionImplAnalyser},
    {ListOfFormalParams,new ListOfFormalParamsAnalyser},
    {BasicTypeName,new BasicTypeNameAnalyser},
    {ArrayTypeName,new ArrayTypeNameAnalyser},
    {TypeName,new TypeNameAnalyser},
    {FunctionContent,new FunctionContentAnalyser},
    {VariableDeclarationList,new VariableDeclarationListAnalyser},
    {VariableInitializationListExt,new VariableInitializationListExtAnalyser},
    {VariableInitialization,new VariableInitializationAnalyser},
    {StmList,new StmListAnalyser},
    {ListOfParamDef,new ListOfParamDefAnalyser},
    {ListOfParamDefExt,new ListOfParamDefExtAnalyser},
    {ParamDef,new ParamDefAnalyser},
    {Expression,new ExpressionAnalyser},
    {Literal,new LiteralAnalyser},
    {NonLiteral,new NonLiteralAnalyser},
    {FunctionCall,new FunctionCallAnalyser},
    {RealParameters,new RealParametersAnalyser},
    {VariableAccess,new VariableAccessAnalyser},
    {Stm,new StmAnalyser},
    {StmListExt,new StmListAnalyser}, // usa o mesmo analisador que o StmList
    {ReturnStm,new ReturnStmAnalyser},
    {IfStm,new IfStmAnalyser},
    {AttrStm,new AttrStmAnalyser}
  });

  try {
    cout << a.getCode() << endl;
  } catch(const string &a) {
    cout << "Error: " << a << endl;
  }

  // Imprime quais funções foram definidas
  /*for (auto i(functionTable.begin()); i != functionTable.end(); i++) {
    cout << i->first << " -> " << get<0>(i->second) << " -> ";
    for (auto j(get<1>(i->second).begin()); j != get<1>(i->second).end(); j++) {
      cout << *j << " ";
    }
    cout << endl;
  }*/

  tree.dispose();

  return 0;
}
