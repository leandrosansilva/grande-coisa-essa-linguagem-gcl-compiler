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

/* TODO: este cara poderia ter outro nome e ser mais genérico, não só para arquivo */

/* TODO: reimplementar este leitor para ler código unicode (utf-x) também.
 * Use std::wifstream e wchar_t
 */

/* TODO: implementar o buffer */

/**
 * Interface com o arquivo de código-fonte
 * 
 * Já elimina linhas em branco e espaços em branco sequenciais
*/
class FileReader: public std::ifstream, public Input
{
public:
  /* Obtem um caractere do arquivo (duh!) */
  char getChar();
  
  /* true se não chegou no fim */
  bool canRead() const;
  
  FileReader(const std::string &filename);
  
  FileReader(const char *filename);
  
  virtual ~FileReader();
  
  /* pega o número de linha do arquivo que está sendo analizada */
  int getLineNumber() const;
  
  /* Pega a coluna da linha que está sendo analizada */
  int getColumnNumber() const;
  
  int getPreviousColumnNumber() const;
  
  bool backOnePosition();
  
  /* Carrega pra memória! */
  bool loadToMemory();
  
  /* Retorna o tamanho, em bytes, do arquivo */
  int getSize();
  
private:
  int _lineNumber;
  int _columnNumber;
  int _previousColumnNumber;
  
  /* nesta string fica todo o conteúdo do arquivo! */
  int8_t *_fileContent;
  
  /* posição atual na string */
  int _curPos;
  
  /* Só para ter armazenado o tamanho do arquivo, em bytes */
  int _size;
};

}

#endif // LEXICAL_FILEREADER_H
