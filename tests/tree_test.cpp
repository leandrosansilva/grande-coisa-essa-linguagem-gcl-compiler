#include <src/syntatic/tree.h>

#include <string>
#include <map>

using namespace Syntatical;
using namespace Common;
using namespace std;

int main()
{
  Token<string> token;

  Tree<string,int>::TreeMap treeMap ({
    {1,new Tree<string,int>(token)},
    {3,new Tree<string,int>(token)}
  });

  Tree<string,int> tree(token);

  Tree<string,int> tree2(2,treeMap);

  Tree<string,int>::TreeMap treeMap2 ({
    {1,new Tree<string,int>(tree)},
    {3,new Tree<string,int>(tree2)}
  });

  Tree<string,int> tree3(2,treeMap2);

}
