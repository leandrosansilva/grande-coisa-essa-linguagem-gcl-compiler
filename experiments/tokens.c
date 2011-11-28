#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef enum {start, a1, a2, b1, b2, b3, c1, c2, c3, d1, d2, d3, e1, e2, f1, f2, g1, g2, h1, h2, i1, i2, j1} State;
typedef enum {inteiro, real, simbolo, comentario, string, identificador, atribuicao} TypeToken;

State automato(char*);
int isFinal(State); //bool
int isSymbol(char); //bool
TypeToken handleState(State); // trata o estado em que o automato parou o processamento


int main (int argc, char** argv) {
  State s = automato(argv[1]);
  if (isFinal(s)) {
    switch (handleState(s)) {
      case inteiro: printf("reconheceu inteiro\n"); break;
      case real: printf("reconheceu real\n"); break;
      case simbolo: printf("reconheceu simbolo\n"); break;
      case comentario: printf("reconheceu comentario\n"); break;
      case string: printf("reconheceu string\n"); break;
      case identificador: printf("reconheceu identificador\n"); break;
      case atribuicao: printf("reconheceu atribuicao\n"); break;
    }
  }
  else printf("token nao reconhecido\n");
  return 0;
}

TypeToken handleState (State st) {
  TypeToken tk;
  switch (st) {
    case a1:
    case a2: tk = identificador; break;
    case b1: tk = inteiro; break;
    case b3: tk = real; break;
    case c3: tk = string; break;
    case d3: tk = comentario; break;
    case d1:
    case d2:
    case f2:
    case g1:
    case g2:
    case h1:
    case h2:
    case i1:
    case i2:
    case j1: tk = simbolo; break;
    case e2: tk = atribuicao; break;
  }
  return tk;
}

int isFinal (State st) {
  return !(st==b2 | st==c1 | st==c2 | st==e1 | st==start);
}

State automato (char* token) {
  State st = start;
  int i = 0;
  switch(token[0]) {
    case '\'': //tratamento de string com ''
      st = c1;
      i=1;
      while ((i<strlen(token)) && (st!=c3)) {
        if (token[i]=='\'') st = c3;
        i++;
      }
      break;
    case '"': //tratamento de string com ""
      st = c2;
      i=1;
      while ((i<strlen(token)) && (st!=c3)) {
        if (token[i]=='"') st = c3;
        i++;
      }
      break;
    case '-':
      st = d1;
      i=1;
      if (token[i] == '-') { //se for um comentario
        st = d3;
        while (i < strlen(token)) i++; // trocar condição no algoritmo original para parar em uma quebra de linha
      } else if (token[i] == '>') st = d2; // se for seta
      break;
    case ':':
      st = e1;
      i = 1;
      if (token[i] == '=') st = e2;
      break;
    case '.':
      st = f1;
      i = 1;
      if (token[i] == '.') st = f2;
      break;
    case '[':
      st = g1;
      i = 1;
      if (token[i] == ']') st = g2;
      break;
    case '<':
      st = h1;
      i = 1;
      if (token[i] == '=') st = h2;
      break;
    case '>':
      st = i1;
      i = 1;
      if (token[i] == '=') st = i2;
      break;
    default:
      if (isalpha(token[0])) st = a1; //se for uma letra
      else if (isSymbol(token[0])) st = j1; //algum outro simbolo da linguagem
      else if (isdigit(token[0])) {
        int parou = 0;
        st = b1;
        for (i = 1; (i < strlen(token)) && (parou == 0); i++) {
          switch (st) {
            case b1:
              if (token[i] == '.') st = b2;
              else if (!isdigit(token[i])) parou = 1;
              break;
            case b2:
              if (!isdigit(token[i])) parou = 1;
              else st = b3;
            case b3:
              if (!(isdigit(token[i]))) parou = 1;
          }
        }
      }
      break;
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
    case '\\': return 1;
    default: return 0;
  }
}

