#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <functional>
#include <set>
#include <list>
#include <sstream>

#include <tuple>

#include "table.h"

using namespace std;

namespace Syntatical {

template<typename NonTerminalT, typename TerminalT>
struct Grammar
{
  struct Symbol
  {
    typedef enum { TERMINAL, NONTERMINAL, ENDMARK, EMPTY, LITERAL} Type;

    Type _type;

    NonTerminalT _nonTerminal;

    TerminalT _terminal;

    bool isEndMark() const
    {
      return _type == ENDMARK;
    }

    bool isEmpty() const 
    {
      return _type == EMPTY;
    }

    bool isTerminal() const
    {
      return _type == TERMINAL;
    }

    bool isNonTerminal() const
    {
      return _type == NONTERMINAL;
    }

    Symbol(const NonTerminalT &nT): _type(NONTERMINAL),_nonTerminal(nT)
    {
    }

    Symbol(const TerminalT &t): _type(TERMINAL), _terminal(t)
    {
    }

    /* símbolo vazio */
    Symbol(): _type(EMPTY)
    {
    }

    bool operator<(const Symbol &other) const
    {
      return  other.isTerminal() && isTerminal() && _terminal < other._terminal
              || other.isNonTerminal() && isNonTerminal() && _nonTerminal < other._nonTerminal
              /* se os dois são diferentes, terminal é sempre menor que não terminal */
              || other.isNonTerminal() && isTerminal()
              /* se é vazio, é menor que qualquer coisa, a não ser que esta outra coisa seja vazio */
              || isEmpty() && !other.isEmpty();
    }

    bool operator>(const Symbol &other) const
    {
      return (!((*this) < other || (*this) == other));
    }

    bool operator!=(const Symbol &other) const
    {
      return !(*this == other);
    }

    /* operadores de comparação */
    bool operator==(const Symbol &other) const
    {
      return other.isTerminal() && isTerminal() && _terminal == other._terminal
             || other.isNonTerminal() && isNonTerminal() && _nonTerminal == other._nonTerminal
             || isEmpty() && other.isEmpty();
    }
  };

  /* uma função que transforma um símbolo numa string */
  typedef function<string(Symbol)> SymbolToString;
  
  /* uma instância da função */
  SymbolToString _symbolToString;

  typedef vector<Symbol> SymbolList;

  typedef set<Symbol> SymbolSet;

  template<typename LeftSide,typename Production>
  struct Rule
  {
    LeftSide _leftSide; 
    Production _production;
    set<int> _abs;

    Rule(const LeftSide &ls, const Production &prod, set<int> abs = set<int>()):
    _leftSide(ls),
    _production(prod),
    _abs(abs)
    {
    }
  };
 
  struct Item
  {
    /* primeiro int é a regra, o segundo é a posiçao do ponto 
     * e por fim o símbolo de lookahead 
    */
    tuple<int,int,Symbol> _item;

    Item(int ruleNumber,int dotNumber, const Symbol &lookahead):
    _item(make_tuple(ruleNumber,dotNumber,lookahead))
    {
    }

    int dot() const
    {
      return get<1>(_item);
    }

    int rule() const
    {
      return get<0>(_item);
    }

    Symbol symbol() const
    {
      return get<2>(_item);
    }

    bool operator<(const Item &other) const
    {
      return _item < other._item;
    }

    bool operator>(const Item &other) const
    {
      return _item > other._item;
    }

    bool operator==(const Item &other) const
    {
      return _item == other._item;
    }

    bool operator!=(const Item &other) const
    {
      return _item != other._item;
    }
  };

  typedef vector<Item> ItemList;

  typedef pair<int,Symbol> Edge;

  /* origem, símbolo, destino  */
  typedef map<Edge,int> EdgeMap;

  /* índice do item, lookahead e regra */
  typedef tuple<int,Symbol,int> ReduceAction;

  typedef set<ReduceAction> ReduceActions;

  typedef tuple<vector<ItemList>,EdgeMap,ReduceActions> CanonicalItems;

  /* uma regra é um não terminal à esquerda, uma lista de símbolos e uma ação semântica */
  typedef vector<Rule<NonTerminalT,SymbolList>> RuleVector;

  typedef Table<Symbol> LR1Table;

  typedef pair<int,Symbol> LR1Key;

  RuleVector _v;

  /* token que representa fim de arquivo */
  TerminalT _EOF;
  
  /* token que representa caractere sem importância */
  TerminalT _invalid;

  Grammar(const SymbolToString symbolToString, const RuleVector &v, const TerminalT &eof, const TerminalT &invalid):
  _v(v),
  _symbolToString(symbolToString),
  _invalid(invalid),
  _EOF(eof)
  {
  }

  virtual ~Grammar()
  {
  }

  /* toda vez que calculo o conjunto first de um símbolo, 
   * adiciono nesta variável, e consulto quando necessário
  */
  map<Symbol,SymbolSet> _firstSymbolSets;

  /* toda vez que calculo o conjunto first de uma lista de símbolos, 
   * adiciono nesta variável, e consulto quando necessário
  */
  map<SymbolList,SymbolSet> _firstSymbolListSets;

  /* regra e posição que já foi checado */
  set<pair<int,int>> _firstChecked;

  LR1Table _table;

  SymbolSet first(const Symbol &symbol)
  {
    if (symbol.isTerminal() || symbol.isEmpty()) {
      return {symbol};
    }

    /* busco na minha tabela de first já calculados */
    auto found(_firstSymbolSets.find(symbol));
    if (found != _firstSymbolSets.end()) {
      // retorna o encontrado!
      return found->second;
    }
      
    SymbolSet f;

    /* para cada produção do símbolo, vai adicionando o first dele em f */
    for (int i(0); i<_v.size(); i++) {
      /* é uma produção do símbolo em questão  */  
      if (_v[i]._leftSide == symbol._nonTerminal) {

        /* se o tamanho da produção é 0, significa que produz vazio */
        if (!_v[i]._production.size()) {
          /* adiciona vazio na lista e sai */
          f.insert(Symbol());
        }

        for (int j(0); j<_v[i]._production.size(); j++) {
          /* o símbolo no qual vou  */
          Symbol s(_v[i]._production[j]);

          /* isso é basicamente para contornar a recursão à esquerda */
          if (_firstChecked.find(pair<int,int>(i,j)) == _firstChecked.end()) {
            _firstChecked.insert(pair<int,int>(i,j));
          } else {
            break;
          }

          /* pega o first do elemento em questão.  */
          SymbolSet pF(first(s));
    
          /* TODO: achar um merge */
          for (auto k(pF.begin()); k != pF.end(); k++) {
            f.insert(*k);
          }

          /* não achei vazio no first. Logo, devo parar */
          if (pF.find(Symbol()) == pF.end()) {
            break;
          }
        }
      }
    }

    /* insere na lista de firsts já calculados para símbolos */
    _firstSymbolSets[symbol] = f;

    return f;
  }

  SymbolSet first(const SymbolList &list)
  {
    /* verifica se já calculou este first */
    auto found(_firstSymbolListSets.find(list));
    if (found != _firstSymbolListSets.end()) {
      // retorna o que achou
      return found->second;
    }
    
    SymbolSet f;

    for(auto i(list.begin()); i != list.end(); i++) {

      SymbolSet sF(first(*i));

      for (auto j(sF.begin()); j != sF.end(); j++) {
        f.insert(*j);
      }

      /* se não há vazio no conjunto achado, sai */
      if (sF.find({}) == sF.end()) {
        break;
      }
    }
    
    /* adiciona o achado */
    _firstSymbolListSets[list] = f;

    return f;
  }

  string itemToString(const Item &item)
  {
    string out;
    
    // imprime o left side
    out += _symbolToString(Symbol(_v[item.rule()]._leftSide)) + " → ";

    // imprime a produção
    for (int i(0); i<_v[item.rule()]._production.size(); i++) {
      if (i == item.dot()) {
        out += ". ";
      }
      out += _symbolToString(_v[item.rule()]._production[i]) + " ";
    }

    // no caso de o ponto estar no final
    if (item.dot() == _v[item.rule()]._production.size()) {
      out += ". ";
    }

    /* imprime o lookahead  */
    out += "  '" + _symbolToString(item.symbol()) + "'";

    return out;
  }

  void printItem(const Item &item)
  {
    cout << itemToString(item) << endl;
  }

  string itemListToString(const ItemList &s)
  {
    string out;
    for (auto i(s.begin()); i != s.end(); i++) {
      out += itemToString(*i) + "\n";
    }
    return out;
  }


  void printItemList(const ItemList &s)
  {
    cout << itemListToString(s) << endl;
  }

  template <typename T>
  void printSymbolList(const T &s)
  {
    for (auto i(s.begin()); i != s.end(); i++) {
      cout << _symbolToString(*i) << ", ";
    }
    cout << endl;
  }
 
  ItemList closure(const ItemList &soi)
  {
    /* copia I em J */
    ItemList s(soi);

    set<Item> used;

    for(int iId(0); iId < s.size(); iId++) {

      Item curItem(s[iId]);

      /* não aplico às regras onde o ponto está no final! */
      if (curItem.dot() >= _v[curItem.rule()]._production.size()) {
        continue;
      }

      /* se o ponto do item não se encntra num não-terminal, nada a ser feito */
      if (!_v[curItem.rule()]._production[curItem.dot()].isNonTerminal()) {
        continue;
      }

      /* concateno todos os símbolos depois do ponto com o lookahead */
      SymbolList withLA(
        _v[curItem.rule()]._production.begin() + curItem.dot() + 1,
        _v[curItem.rule()]._production.end()
      );

      /* concateno o lookahead */
      withLA.push_back(curItem.symbol());

      /* O first do conjunto acima diz quais símbolos vêm após ele  */
      SymbolSet f(first(withLA));

      /* limpo o cara que guarda as informações do first: FIXME: side-effect */
      _firstChecked.clear();

      /* comparo o item com todas as regras da gramática  */
      for (int i(0); i < _v.size(); i++) {

        /* só aplico às regras cuja símbolo da esquerda seja igual ao que tenho em mãos */
        if (_v[i]._leftSide != _v[curItem.rule()]._production[curItem.dot()]._nonTerminal) {
          continue;
        }

        /* pra cada símbolo achado em first, se não o usei, adiciono na closure */
        for (auto w(f.begin()); w != f.end();w++) {
          Item item(i,0,*w);

          /* Se já foi usado, vou para o próximo  */
          if (used.find(item) != used.end()) {
            continue;
          }

          /* adiciona o novo item, mas com o ponto no começo */
          s.push_back(item);

          used.insert(item);
        }
      }
    }

    return s;
  }

  ItemList goTo(const ItemList &its,const Symbol &x)
  {
    ItemList j;

    for (auto it(its.begin()); it != its.end(); it++) {

      /* se o ponto está no fim ou a produção é vazia, não devo aplicar */
      if (_v[it->rule()]._production.size() == it->dot() || 
          _v[it->rule()]._production.size() == 0) {
        continue;
      }

      if (_v[it->rule()]._production[it->dot()] == x) {
        j.push_back(Item(it->rule(),it->dot()+1,it->symbol()));
      }
    }

    return closure(j);
  }

  CanonicalItems items()
  {
    vector<ItemList> f {closure({{0,0,{_invalid}}})};

    EdgeMap edges;

    bool firstTime(true);

    ReduceActions reduceActions;

    /* enquanto houver elementos na lista, remove e joga na lista final */
    for (int itemIndex(0); itemIndex<f.size(); itemIndex++ ) {

      /* pega o primeiro da lista, remove-o e insere na lista final */
      ItemList s(f[itemIndex]);

      /* símbolos já usados para goto */
      SymbolSet used;

      /* para cada item no conjunto */
      for (auto item(s.begin()); item != s.end(); item++) {

        /* se o ponto está no final, é uma ação de redução */
        if (item->dot() == _v[item->rule()]._production.size()) {
          reduceActions.insert(ReduceAction(itemIndex,item->symbol(),item->rule()));
          continue;
        }
        
        // Símbolo da transição
        Symbol edgeSymbol(_v[item->rule()]._production[item->dot()]);

        // se o símbolo que estou analisando for o de final de arquivo, salto pro final
        if (edgeSymbol == Symbol(_EOF)) {
          continue;
        }

        /* já foi usado */
        if (used.find(edgeSymbol) != used.end()) {
          continue;
        }

        // defino como símbolo usado
        used.insert(edgeSymbol);

        // faço o goto com o símbolo
        ItemList j(goTo(s,edgeSymbol));

        /* se o goto aplicado no elemento não resultou num novo conjunto, saio */
        if (!j.size()) {
          continue;
        }

        // o iterator do destino
        auto dst(find(f.begin(),f.end(),j));

        /* o índice do conjunto destino */
        int dstIndex;

        // se o elemento destino foi encontrado na lista dos finais, pega seu índice
        if (dst != f.end()) {
          dstIndex = distance(f.begin(),dst);

          // Insiro no mapa de vértices
          edges[pair<int,Symbol>(itemIndex,edgeSymbol)] = dstIndex; 

          continue;
        }

        dstIndex = f.size();

        /* insere a closure no conjunto final de closures */
        f.push_back(j);

        // Insiro no mapa de vértices
        edges[pair<int,Symbol>(itemIndex,edgeSymbol)] = dstIndex; 
      }
    }
    
    return CanonicalItems(f,edges,reduceActions);
  }

  void generateGraph()
  {
    CanonicalItems cItems(items());

    cout << "digraph MyGrammar { " << endl;

    map<ItemList*,int> labels;

    int lCount(0);

    for (auto s(get<0>(cItems).begin()); s != get<0>(cItems).end(); s++, lCount++) {

      string dsts;

      cout << "  " << "c" << lCount << " [shape=rect,label=\"";
      
      cout << lCount << "\\n";
      
      //para cada item
      for (auto item(s->begin()); item != s->end(); item++) {
        // o item impresso
        cout << itemToString(*item) << "\\n";
      }

      cout << "\"];" << endl;

      cout << dsts;
    }

    for (auto e(get<1>(cItems).begin()); e != get<1>(cItems).end(); e++ ) {
      cout << "  c" << e->first.first << " -> c" 
           << e->second << " [label=\"" 
           << _symbolToString(e->first.second) << "\"]; "<< endl;
    }

    cout << "}" << endl;
  }

  LR1Table &createTable()
  {
    CanonicalItems cItems(items());

    EdgeMap edges(get<1>(cItems));
    
    ReduceActions reduceActions(get<2>(cItems));

    /* insiro as regras de ACCEPT e REDUCE primeiro, 
     * pois assim as ações de shift poderão sobreescrevê-las
     * sim, isso é uma abordagem porca.
    */
    for (auto r(reduceActions.begin()); r != reduceActions.end(); r++) {
      _table[LR1Key(get<0>(*r),get<1>(*r))] = {REDUCE,get<2>(*r)};
    }

    for (auto e(edges.begin()); e != edges.end(); e++) {
      if (e->first.second.isNonTerminal()) {
        /* se é um não terminal, faz goto */
        _table[LR1Key(get<0>(*e).first,get<0>(*e).second)] = {GOTO,get<1>(*e)};

      } else if (e->first.second.isTerminal()) {
        /* se é um terminal, faz shift */
        _table[LR1Key(get<0>(*e).first,get<0>(*e).second)] = {SHIFT,get<1>(*e)};
      }
    }

    /* Por fim, inclui o estado de aceitação, que é sempre quando, em 1, acaba o arquivo */
    _table[LR1Key(1,_EOF)] = {ACCEPT};

    return _table; 
  }

  void printTable()
  {
    for (auto r(_table.begin()); r != _table.end(); r++) {
      cerr << get<0>(r->first) << " " << _symbolToString(get<1>(r->first))
           << " " << _table.actionToString(r->second._action) << " " << r->second._value << endl;
    }
  }
};

}
#endif
