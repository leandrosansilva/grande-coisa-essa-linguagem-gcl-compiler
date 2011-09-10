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

using namespace std;

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
      bool t(other.isTerminal() && isTerminal() && _terminal < other._terminal);
      bool n(other.isNonTerminal() && isNonTerminal() && _nonTerminal < other._nonTerminal);

      /* se os dois são diferentes, terminal é sempre menor que não terminal */
      bool d(other.isNonTerminal() && isTerminal());

      /* se é vazio, é menor que qualquer coisa, a não ser que esta outra coisa seja vazio */
      bool e(isEmpty() && !other.isEmpty());

      return t || n || d || e;
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
      /* TODO: reescrever com operador ternário */
      bool t(other.isTerminal() && isTerminal() && _terminal == other._terminal);

      bool n(other.isNonTerminal() && isNonTerminal() && _nonTerminal == other._nonTerminal);

      bool e(isEmpty() && other.isEmpty());

      return t || n || e;
    }
  };

  /* uma função que transforma um símbolo numa string */
  typedef function<string(Symbol)> SymbolToString;
  
  /* uma instância da função */
  SymbolToString _symbolToString;

  typedef vector<Symbol> SymbolList;

  template<typename LeftSide,typename Production, typename Action>
  struct Rule
  {
    LeftSide _leftSide; 
    Production _production; 
    Action _action;

    Rule(const LeftSide &ls, const Production &prod, const Action &act):
    _leftSide(ls),
    _production(prod),
    _action(act)
    {
    }
  };
 
  struct Item
  {
    int _rule;
    int _dot;
    Symbol _s;
    Item(int ruleNumber,int dot, const Symbol &s):
    _rule(ruleNumber),
    _dot(dot),
    _s(s)
    {
    }

    bool operator<(const Item &other) const
    {
      return !(*this == other || *this > other);
    }

    bool operator>(const Item &other) const
    {
      if (_s > other._s) {
        return true;
      }

      if (_s < other._s) {
        return false;
      }

      if ((_rule * 1000 + _dot) > (other._rule * 1000 + other._dot)) {
        return true;
      }
      
      return false;
    }

    bool operator==(const Item &other) const
    {
      return _rule == other._rule && _dot == other._dot && _s == other._s;
    }

    bool operator!=(const Item &other) const
    {
      return !(*this == other);
    }
  };

  typedef vector<Item> ItemList;

  typedef set<Symbol> SymbolSet;

  typedef pair<list<ItemList>,map<Item,ItemList *>> CanonicalItems;

  typedef vector<Rule<NonTerminalT,SymbolList,int>> RuleVector;

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

  /* regra e posição que já foi checado */
  set<pair<int,int>> _firstChecked;

  SymbolSet first(const Symbol &symbol)
  {
    if (symbol.isTerminal() || symbol.isEmpty()) {
      return {symbol};
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

    return f;
  }

  SymbolSet first(const SymbolList &list)
  {
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

    return f;
  }

  string itemToString(const Item &item)
  {
    string out;
    
    // imprime o left side
    out += _symbolToString(Symbol(_v[item._rule]._leftSide)) + " → ";

    // imprime a produção
    for (int i(0); i<_v[item._rule]._production.size(); i++) {
      if (i == item._dot) {
        out += ". ";
      }
      out += _symbolToString(_v[item._rule]._production[i]) + " ";
    }

    // no caso de o ponto estar no final
    if (item._dot == _v[item._rule]._production.size()) {
      out += ". ";
    }

    /* imprime o lookahead  */
    out += ", #'" + _symbolToString(item._s) + "'";

    return out;
  }

  void printItem(const Item &item)
  {
    cout << itemToString(item) << endl;
  }

  void printItemList(const ItemList &s)
  {
    for (int i(0); i< s.size(); i++) {
      printItem(s[i]);
    }
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
      if (curItem._dot >= _v[curItem._rule]._production.size()) {
        continue;
      }

      /* se o ponto do item não se encntra num não-terminal, nada a ser feito */
      if (!_v[curItem._rule]._production[curItem._dot].isNonTerminal()) {
        continue;
      }

      /* concateno todos os símbolos depois do ponto com o lookahead */
      SymbolList withLA(
        _v[curItem._rule]._production.begin() + curItem._dot + 1,
        _v[curItem._rule]._production.end()
      );

      /* concateno o lookahead */
      withLA.push_back(curItem._s);

      /* O first do conjunto acima diz quais símbolos vêm após ele  */
      SymbolSet f(first(withLA));

      /* limpo o cara que guarda as informações do first: FIXME: side-effect */
      _firstChecked.clear();

      /* comparo o item com todas as regras da gramática  */
      for (int i(0); i < _v.size(); i++) {

        /* só aplico às regras cuja símbolo da esquerda seja igual ao que tenho em mãos */
        if (_v[i]._leftSide != _v[curItem._rule]._production[curItem._dot]._nonTerminal) {
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
      if (_v[it->_rule]._production.size() == it->_dot || 
          _v[it->_rule]._production.size() == 0) {
        continue;
      }

      if (_v[it->_rule]._production[it->_dot] == x) {
        j.push_back(Item(it->_rule,it->_dot+1,it->_s));
      }
    }

    return closure(j);
  }

  CanonicalItems items()
  {
    list<ItemList> f, t({closure({{0,0,{_invalid}}})});

    /* guarda os items que eu já usei para fazer goto e para onde levam*/
    map<Item,ItemList *> usedItems;

    bool firstTime(true);

    /* enquanto houver elementos na lista, remove e joga na lista final */
    while (t.size() || firstTime) {

      /* pega o primeiro da lista, remove-o e insere na lista final */
      ItemList s(t.front());
      t.pop_front();

      /* de o conjunto já foi produzido, sai */
      auto found(find(f.begin(),f.end(),s));

      if (found != f.end() && !firstTime) {
        continue;
      }
  
      f.push_back(s);

      firstTime = false;

      /* para cada item no conjunto */
      for (auto item(s.begin()); item != s.end(); item++) {

        /* se o ponto está no final, não faz nada */
        if (item->_dot == _v[item->_rule]._production.size()) {
          continue;
        }

        // para cada cara depois do ponto da regra em questão, faz o goto dele
        ItemList j(goTo(s,_v[item->_rule]._production[item->_dot]));

        /* se o goto aplicado no elemento não resultou num novo conjunto, saio */
        if (!j.size()) {
          continue;
        }

        /* de o conjunto já foi produzido, sai */
        auto dst(find(f.begin(),f.end(),j));

        if (dst != f.end()) {
          continue;
        }

        /* insere a closure no conjunto final de closures */
        t.push_back(j);
      }
    }
    return {f,usedItems};
  }

  void generateGraph()
  {
    cout << "digraph MyGrammar { " << endl;
    CanonicalItems canonical(items());

    map<ItemList*,int> labels;

    int lCount(0);

    //dado um ItemList, retorna sua label
    function<string(ItemList *)> getLabel([&lCount,&labels](ItemList *itemList) -> string {
      stringstream ssLabel;

      if (labels.find(itemList) == labels.end()) {
        labels[itemList] = lCount;
        ssLabel << "c" << lCount;
        lCount++;
      } else { 
        // se encontrei, pego seu valor
        ssLabel << "c" << labels[itemList];
      }

      return ssLabel.str();
    });

    for (auto s(canonical.first.begin()); s!= canonical.first.end(); s++) {

      string dsts;

      //cout << "  " << getLabel(*s) << " [shape=rect,label=\"";
      cout << "  " << "c" << lCount++  << " [shape=rect,label=\"";
      
      //para cada item
      for (auto item(s->begin()); item != s->end(); item++) {

        auto d(canonical.second.find(*item));
      
        //pra onde goto deste item leva?
        ItemList *dst(d->second);

        // o item impresso
        cout << itemToString(*item) << "\\n";

        //se não aponta para lugar algum (ponto no final), 
        // não devo analisar destinos inexistentes 
        //
        if (d == canonical.second.end()) {
          continue;
        }
      
        //dsts += "  " + getLabel(*s) + " -> " + getLabel(dst);
        //dsts += " [label=\"" + _symbolToString(_v[item->_rule]._production[item->_dot]) + "\"];";
      }

      cout << "\"];" << endl;

      cout << dsts;
    }

    cout << "}" << endl;
  }

};
#endif
