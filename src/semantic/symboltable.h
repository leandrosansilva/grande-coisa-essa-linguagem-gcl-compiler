#ifndef SEMANTIC_TABLE_H
#define SEMANTIC_TABLE_H

#include <src/syntatic/tree.h>
#include <src/syntatic/grammar.h>

/* Qual a ideia da tabela de símbolos?
 * Para variável, é string(nome da variável) -> tipo(um ponteiro para uma entrada na tabela de símbolos)
 * Para símbolo, é uma string(nome do tipo) -> uma estrutura que diga se é
 *  -> array -> de qual tipo e qual tamanho
 *  -> estrutura(tupla) -> lista de tipos (um ponteiro para um elemento da mesma tabela de tipos)
 *  -> intervalo (início, fim e qual o tipo)
 *  -> ou só um "alias" para um tipo primitivo
 * 
 * Uma estratégia para os tipos built-ins é inserí-los na tabela de tipos
 * Insiro um tipo na tabela de tipos sempre que ele for declarado
 * Insiro uma variável na tabela de símbolos sempre que ela for declarada
 * toda vez que uma variável for usada, olho ela na tabela de símbolos. Se ela não existir, é erro
 * 
 * Já uma tabela de funções possui como chave o nome da função
 * e como atributos uma lista com uma estrutura {valor|referencia -> tipo}
*/

template<typename KeyT, typename ValueT>
struct SymbolTable
{
};

template<typename KeyT, typename ValueT>
struct TypeTable
{
};

#endif
