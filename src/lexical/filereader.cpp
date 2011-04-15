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

FileReader::FileReader(const std::string &filename):
std::ifstream(filename.c_str()),
_lineNumber(1),
_columnNumber(1)
{
}

FileReader::FileReader(const char* filename): 
std::ifstream(filename),
_lineNumber(1),
_columnNumber(1)
{
}

char FileReader::getChar()
{
  /* TODO: melhorar isso lendo grandes buffers por vez
   * Ou mesmo o arquivo todo para a memória
   */
  char c;
  
  read(&c,sizeof(char));
  
  if (c == '\n') {
    _columnNumber = 1;
    _lineNumber++;
  } else {
    _columnNumber++;
  }

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

bool FileReader::backOnePosition()
{
  seekg(-1,std::ios::cur);
}
