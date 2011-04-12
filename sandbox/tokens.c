#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef enum {start, a1, a2, b1, b2, b3, c1, c2, c3, d1, d2, d3, e1, e2, f1, f2, g1, g2, h1, h2, i1, i2, j1} State;

State automato(char*);
int isFinal(State); //bool
int isSymbol(char); //bool

int main (int argc, char** argv) {
  int i;
  for (i=1; i<=argc; i++) {
    automato(argv[i]);
  }
  return 0;
}

int isFinal (State st) {
  return !(st==b2 | st==c1 | st==c2 |st==e1);
}

State automato (char* token) {
  State st = start;
  int i = 0;
  switch(token[0]) {
    case '\'': {
      st = c1;
      for (i=1; i<strlen(token); i++) {
        if (token[i]=='\'') {
          st = c3;
        } else if (st == c3); //implementar para o estado c3
      }
      break;}
    case '"': {
      st = c2;
      break;}
    case '-': {
      st = d1;
      break;}
    case ':': {
      st = e1;
     break;}
    case '.': {
      st = f1;
      break;}
    case '[': {
      st = g1;
      break;}
    case '<': {
      st = h1;
      break;}
    case '>': {
      st = i1;
      break;}
    default: {
      if (isalpha(token[0]) { //se for uma letra
        st = a1;
      } else if (isSymbol(token[0]) {  //algum outro simbolo da linguagem
        st = j1;
      }
    }
  }
  return st;
}

int isSymbol(char c) {
  switch(c) {
    case '=':
    case ',':
    case '(':
    case ')':
    case ']':
    case '#':
    case '&':
    case '|':
    case '~':
    case '+':
    case '*':
    case '/':
    case '\': return 1;
    default: return 0;
  }
}

