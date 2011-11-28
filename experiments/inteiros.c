#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef enum {A, B} state;

/*void handle_digit (state st){
  switch state {
  A: 
  }
} */

void handle_other(state* st){
  if (*st == A) *st = B;
}

state init() {
  return A;
}

void analyse(char digit, state* st){
  if (isdigit(digit) == 0) {
    handle_other(st);
  }
}

int handle_state(state st) {
  switch (st) {
    case A: return 1;
    case B: return 0;
  }
}

int main(int argc, char** argv) {
  char *c = argv[1];
  state s = init();
  int i = 0;
  for (i = 0; i < strlen(c); i++) {
    analyse (c[i], &s);
  }
  if (handle_state(s) == 1) printf("Inteiro valido\n");
  else printf ("falhou\n");
  return 0;
}

