#include <iostream>
using namespace std;

enum token {
  IF, THEN, ELSE, BEGIN, END, PRINT, SEMI, NUM, EQ
};

void S();
void L();
void E();
token getToken();
void error();
void advance();


enum token tok;

void advance()
{
  tok = getToken();
}

void eat(enum token t)
{
  if (tok == t) 
    advance();
  else
    error();
}

void S()
{
  switch (tok) {
    case IF: 
      eat(IF); E(); eat(THEN); S(); eat(ELSE); S(); 
    break;
    case BEGIN:
      eat(BEGIN); S(); L(); 
    break;
    case PRINT:
      eat(PRINT); S(); L(); 
    break;
    default:
      error();
  }
}

void L()
{
  switch (tok) {
    case END: 
      eat(END);
    break;
    case SEMI:
      eat(SEMI); S(); L(); break;
    break;
    default:
      error();
  }
}

void E()
{
  eat(NUM); eat(EQ); eat(NUM);
}

static const token str[] = { NUM, SEMI };
static int index = 0;
const int size = sizeof(str) / sizeof(token);

token getToken()
{
  return str[index++];  
}

void error()
{
  cerr << "erro" << endl;
}

int main(int argc, char **argv)
{
  while (i<size){
        
  }  
}
