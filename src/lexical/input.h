#ifndef LEXICALINPUT_H
#define LEXICALINPUT_H
namespace Lexical {
class Input
{
public:
  /* Obtem o próximo caractere */
  virtual char getChar() = 0;
  
  /* true se não chegou no fim */
  virtual bool canRead() const = 0;
  
  /* pega o número de linha da entrada que está sendo analizada */
  virtual int getLineNumber() const = 0;
  
  /* Pega a coluna da linha que está sendo analizada */
  virtual int getColumnNumber() const = 0;
  
  virtual int getPreviousColumnNumber() const = 0;
  
  virtual bool back(int = 1) = 0;
  
  /* Retorna o tamanho, em bytes, do arquivo */
  virtual int getSize() const = 0;
  
  /* posição atual na stream */
  virtual int getPos() const = 0;
};
}
#endif