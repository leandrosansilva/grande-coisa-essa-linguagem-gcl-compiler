#ifndef GCLTOKENTYPE_H
#define GCLTOKENTYPE_H
/*
  * Palavras reservadas da GCL:
  * 
  * module private end const Boolean integer begin typedef array range
  * proc val ref return write read if fi do od true false forall llarof skip
  * 
  * Símbolos:
  * 
  * .   =   :=  --  ,   ;   (   ) []  [   ]   ->  #
  * < > <=  >=  &   |   ~   +   -   *   /   \  ..
  * 
  * Comentários:
  * -- comentário
  *
  * Um programa é uma sequência de módulos. Definição de módulo:
  * <module> ->
  * module identifier
  * <definitionPart>
  * private
  * <block> .
*/
typedef enum {
  /* Nenhum Token, útil como resultado de busca? */
  None = 0,
  
  /* Palavras reservadas */
  Module,
  Private,
  End,
  Const,
  Boolean,
  Integer,
  Begin,
  Typedef,
  Array,
  Range,
  Proc,
  Val,
  Ref,
  Return,
  Write,
  Read,
  If,
  Fi,
  Do,
  Od,
  True,
  False,
  Forall,
  Llarof,
  Skip,
  
  /* Símbolos */
  Dot, // .
  Equal, // =
  Assign, // :=
  Comment, // --
  Comma, // ,
  Semicolon, // ;
  LParentesis, // (
  RParentesis, // )
  LBracket, // [
  RBracket, // ]
  Else, // [] // else
  Then, // -> // then
  Sharp, // #
  Lesser, // <
  Greater, // >
  LessEqual, // <=
  GreaterEqual, // >=
  And, // &
  Or, // |
  Til, // ~
  Plus, // +
  Minus, // -
  Asterisk, // *
  Slash, // /
  BackSlash, // \\
  TwoDot, // ..
 
  /* Tipos constantes, números, etc. */
  ClassInteger,
  ClassReal,
  ClassString,
  ClassId,
  ClassComment,
  ClassSpaces
  
} TokenType;

#endif