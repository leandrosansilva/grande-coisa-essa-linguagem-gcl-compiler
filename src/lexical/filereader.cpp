/*
    <one line to give the program's name and a brief idea of what it does.>
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


#include "filereader.h"

#include <fstream>

#include <iostream>

using namespace Lexical;

FileReader::FileReader(const std::string& filename):
std::ifstream(filename.c_str()),
_lineNumber(1),
_columnNumber(0),
_ignoreBlank(true)
{
}

FileReader::FileReader(const char* filename): 
std::ifstream(filename),
_lineNumber(1),
_columnNumber(0),
_ignoreBlank(true)
{
}

/* FIXME: não está indicando colunas corretamente! */
char FileReader::getChar()
{
  /* TODO: melhorar isso lendo grandes buffers por vez */
  
  /* inicializo o caractere com uma quebra de 
   * linha para executar o laço ao menos uma vez 
   */
  char c('\n');
  
  /* contador de colunas */
  int columnCounter(0);
  
  while (c == '\n' && canRead()) {
    /* contador de caracteres em branco */
    int blankCounter(0);

    /* laço que consome os espaços em branco */
    do {
      read(&c,sizeof(char));
      blankCounter++;
    } while ((c == '\t' || c == ' ') && ignoreBlank());
    
    /* incrementa o tanto de colunas que deslocou.
     * é, no mínimo, 1, pois leu um caractere
    */
    columnCounter += blankCounter;

    _lineNumber++;
  }
  
  std::cout << "column: " << (int)_columnNumber << ", andei " << columnCounter << " brancos antes de " << c << std::endl; 
  
  /* se o contador maior que um,
   * significa que li mais de um caractere, ou seja, 
   * li um quebra de linha
   * Caso contrário, li um caractere comum, 
   * logo devo somente incrementar a coluna
  */
  
  _columnNumber = (columnCounter >= 2) ? columnCounter: _columnNumber + 1;
 
  /* como tenho um incremento de linhas a mais, decremento */
  _lineNumber--;
 
  return c;
}

bool FileReader::canRead() const
{
  return !eof();
}

uint16_t FileReader::getLineNumber() const
{
  return _lineNumber;
}

uint16_t FileReader::getColumnNumber() const
{
  return _columnNumber;
}

bool FileReader::ignoreBlank() const
{
  return _ignoreBlank;
}
