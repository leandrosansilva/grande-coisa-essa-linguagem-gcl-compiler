#include <src/syntatic/tree.h>

#include <string>
#include <map>
#include <iostream>

using namespace Syntatical;
using namespace Common;
using namespace std;

int main()
{
  Token<string> token;

  Tree<string,int>::TreeMap treeMap ({
    {35,new Tree<string,int>(token)},
    {17,new Tree<string,int>(token)}
  });

  Tree<string,int> tree(token);

  Tree<string,int> tree2(2,treeMap);

  Tree<string,int>::TreeMap treeMap2 ({
    {42,new Tree<string,int>(tree)},
    {69,new Tree<string,int>(tree2)}
  });

  Tree<string,int> tree3(2,treeMap2);

  cout << tree3.toString() << endl;

}
