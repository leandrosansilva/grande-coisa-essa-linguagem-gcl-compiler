#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Automato que trata uma string do tipo [0-9]+[Ll] */

typedef enum {A, B, Final} states;

static char *inputtext;
static char matchedtext[256];
static int pos = 0;
static int matchpos = 0;

int canread()
{
  return pos <= strlen(inputtext);
}

char getnewchar()
{
  return inputtext[pos++];
}

int fail(int pos)
{
  printf("casei só %s e falhei na posição %d\n",matchedtext,pos);
  exit(1);
}

int accept_token()
{
  printf("Ok: %s\n",matchedtext);
  exit(0);
}

int retract()
{
}

static states state;

int main(int argc, char **argv)
{
  inputtext = argv[1];

  state = A;

  while (canread()) {
    char ch = getnewchar();
    
    switch (state) {
      case A:
        /* [0-9] */
        if (isdigit(ch)) {
          matchedtext[matchpos++] = ch;
          state = B;
        } else
          fail(pos);
      break;

      case B:
        /* [0-9] */
        if (isdigit(ch)) {
          /* permanece no mesmo estado */
          matchedtext[matchpos++] = ch;
          state = B;
        
        } else if (ch == 'L' || ch == 'l') {
          state = Final;
          matchedtext[matchpos++] = ch;
        } else
          fail(pos);
      break;

      /* Estado final, tudo ok! */
      case Final:
          accept_token();
      break;
      
      default:
        printf("Entrei no estado default\n");
    }
    
  }
}
