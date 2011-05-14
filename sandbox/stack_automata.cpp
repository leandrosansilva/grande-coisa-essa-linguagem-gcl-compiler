#include <stack>
#include <string>
#include <iostream>

using namespace std;

template<typename SymbolType, typename StackSymbolType>
class StackTransition
{
  SymbolType _read;
public:
  StackTransition(const SymbolType &symbol_);
};

using namespace std;

enum State {A, B};

typedef stack<State> Stack(A);

/* Preciso de uma pilha de símbolos */

/* string de a's e b's onde o número de a's é o mesmo que o de b's */
int main(int argc, char **argv)
{
  string input(argv[1]);
  Stack stack;
  
  for (string::const_iterator it(input.begin());it != input.end(); it++) {
    
    /* caso a pilha esteja vazia */
    if (stack.empty()) {
      switch(*it) {
        case 'a':
          stack.push(A);
        break;
        case 'b':
          stack.push(B);
        break;
      }
    } else {
      const State cur(stack.top());
      switch (cur) {
        case A:
          if (*it == 'a') 
            stack.push(A);
          else if (*it == 'b')
            stack.pop();
        break;
        
        case B:
          if (*it == 'b')
            stack.push(B);
          else if (*it == 'a')
            stack.pop();
        break;
      }
    }
  }
  
  cout << (stack.size() ? "Falhou" : "Casou" ) << endl; 
  
  return 0;
}
