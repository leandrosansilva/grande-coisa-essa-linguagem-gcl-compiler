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

#include <ctype.h>

using namespace Lexical;

FileReader::FileReader(const std::string &filename, bool iBlank):
std::ifstream(filename.c_str()),
_lineNumber(1),
_columnNumber(0),
_ignoreBlank(iBlank)
{
}

FileReader::FileReader(const char* filename, bool iBlank): 
std::ifstream(filename),
_lineNumber(1),
_columnNumber(0),
_ignoreBlank(iBlank)
{
}

char FileReader::getChar()
{
  /* TODO: melhorar isso lendo grandes buffers por vez */
  /* TODO: quando há varios espaços, retornar um só */
  
  /* inicializo o caractere com uma quebra de 
   * linha para executar o laço ao menos uma vez 
   */
  char c('\n');
  
  /* contador de colunas */
  int columnCounter(0);
  
  /* conta quantas linhas serão andadas pra ler um char */
  int origLineNumber(_lineNumber);
  
  while (c == '\n' && canRead()) {
    
    
    /* se estou a partir da segunda iteração,
     * ou seja, andei mais de uma linha pra pegar um char
     * coluna volta pra zero
    */
    if (origLineNumber < _lineNumber) {
      _columnNumber = 0;
      columnCounter = 0;
    }
      
    /* contador de caracteres em branco */
    int blankCounter(0);
      
    /* laço que consome os espaços em branco */
    read(&c,sizeof(char));
    while ((isblank(c)) && ignoreBlank()) {
      read(&c,sizeof(char));
      blankCounter++;
    }
    
    /* incrementa o tanto de colunas que deslocou.
     * é, no mínimo, 1, pois leu um caractere
    */
    columnCounter += blankCounter;

    _lineNumber++;
  }
  
  /* como tenho um incremento de linhas a mais, decremento */
  _lineNumber--; 

  /* incrementa o número de colunas com o que eu li */
  _columnNumber += columnCounter + 1;
 
  return c;
}

bool FileReader::canRead() const
{
  return !eof();
}

int FileReader::getLineNumber() const
{
  return _lineNumber;
}

int FileReader::getColumnNumber() const
{
  return _columnNumber;
}

bool FileReader::ignoreBlank() const
{
  return _ignoreBlank;
}

bool FileReader::disableIgnoreBlank()
{
  _ignoreBlank = false;
}

bool FileReader::enableIgnoreBlank()
{
  _ignoreBlank = true;
}
