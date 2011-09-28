/*
    <Lê caracteres de um arquivo>
    Copyright (C) 2011  Leandro Santiago <leandrosansilva@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LEXICAL_FILEREADER_H
#define LEXICAL_FILEREADER_H

#include <fstream>
#include <string>

#include <stdint.h>

#include "input.h"

namespace Lexical {

/* TODO: implementar o buffer para não ler o arquivo todo para a memória */

/*
 * Interface com o arquivo de código-fonte
*/
class FileReader: public std::ifstream, public Input
{
public:
  /* Obtem um caractere do arquivo (duh!) */
  char getChar();
  
  /* true se não chegou no fim */
  bool canRead() const;
  
  FileReader(const std::string &filename);
  
  virtual ~FileReader();
  
  /* pega o número de linha do arquivo que está sendo analizada */
  int getLineNumber() const;
  
  int getColumnNumber() const;
  
  /* volta n posições na entrada */
  bool back(int = 1);
  
  /* Carrega o arquivo todo pra memória! */
  bool loadToMemory();
  
  /* Retorna o tamanho, em bytes, do arquivo */
  int getSize() const;
  
  /* Posição na entrada */
  int getPos() const;
  

  
private:
  int _lineNumber;
  int _columnNumber;
  
  /* nesta string fica todo o conteúdo do arquivo! 
   * TODO: mudar isso para pedaços por demanda
  */
  char *_fileContent;
  
  /* posição atual na string */
  int _curPos;
  
  /* Só para ter armazenado o tamanho do arquivo, em bytes */
  int _size;
};

}

#endif // LEXICAL_FILEREADER_H
