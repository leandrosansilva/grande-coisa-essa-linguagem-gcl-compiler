#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define A 100
# define B 101
# define C 102

static char *cadeia;
static int pos = 0;

/*int isdigit(char c)
{
  printf("%c\n",c);
  return c >= 30 && c <= 39;
}*/

int canread()
{
  return pos <= strlen(cadeia);
}

char getnewchar()
{
  return cadeia[pos++];
}

int fail()
{
  printf("falhei\n");
  exit(1);
}

int accept_token()
{
  printf("Ok!\n");
  exit(0);
}

int retract()
{
}

static int state;


int main(int argc, char **argv)
{
  cadeia = argv[1];

  char ch;

  ch = getnewchar();

  if (ch == '+' || ch == '-' || isdigit(ch)) 
    state = A;

  int c = 1;

  while (canread() && c ) {
    switch (state) {
      case A: 
        printf("está no estado A\n");
        ch = getnewchar();
        if (ch == '.')
          state = B;
        else {
          if (isdigit(ch)); 
          else 
            fail();
        }
      break;

      case B:
        printf("está no estado B\n");
        ch = getnewchar();
        if (isdigit(ch)) 
          state = C;
        else
          fail();
      break;

      case C:
        printf("está no estado C\n");
        ch = getnewchar();
        if (!isdigit(ch)) {
          retract();
          accept_token();
        }
      break;
      default:
        printf("está no estado Lixo\n");
        c = 0;
    }
    
  }
}
