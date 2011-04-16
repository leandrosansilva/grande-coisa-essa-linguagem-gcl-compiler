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
std::ifstream(filename.c_str(),std::ios::ate),
_lineNumber(1),
_columnNumber(0),
_curPos(0),
/* Preciso de um caractere a mais no final do arquivo, por isso +1 */
_size(tellg())
{
  loadToMemory();
}

FileReader::FileReader(const char* filename): 
std::ifstream(filename,std::ios::ate),
_lineNumber(1),
_columnNumber(1),
_curPos(0),
_size(tellg())
{
  loadToMemory();
}

int FileReader::getSize()
{
  return _size;
}

bool FileReader::loadToMemory()
{
  _fileContent = new char[_size];
  seekg(0,std::ios::beg);
  read(_fileContent,_size);
}

char FileReader::getChar()
{
  char c(_fileContent[_curPos++]);
 
  /* faço um "backup" da coluna anterior */
  _previousColumnNumber = _columnNumber;
  
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
  return _curPos < _size;
}

int FileReader::getLineNumber() const
{
  return _lineNumber;
}

int FileReader::getColumnNumber() const
{
  return _columnNumber;
}

int FileReader::getPreviousColumnNumber() const
{
  return _previousColumnNumber;
}

bool FileReader::backOnePosition()
{
  if (_fileContent[--_curPos] == '\n') {
    _lineNumber--;
    _columnNumber = _previousColumnNumber;
  }
}

FileReader::~FileReader()
{
  delete _fileContent;
}
