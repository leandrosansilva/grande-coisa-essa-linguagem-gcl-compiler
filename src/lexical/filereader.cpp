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

FileReader::FileReader(const std::string &filename):
std::ifstream(filename.c_str(),std::ios::ate),
_lineNumber(1),
_curPos(0),
/* Preciso de um caractere a mais no final do arquivo, por isso +1 */
_size(tellg())
{
  loadToMemory();
}

int FileReader::getSize() const
{
  return _size;
}

bool FileReader::loadToMemory()
{
  _fileContent = new char[_size];
  seekg(0,std::ios::beg);
  read((char *)_fileContent,_size);
}

char FileReader::getChar()
{
  /* Pega um caractere da entrada */
  char c(_fileContent[_curPos++]);
 
  if (c == '\n') {
    _lineNumber++;
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

bool FileReader::back(int pos)
{
  /* A posição para onde devo voltar */
  int backPos(_curPos - pos);
  
  while (_curPos > backPos) {
    if (_fileContent[_curPos] == '\n')
      _lineNumber--;
    
    _curPos--;
  }
}

FileReader::~FileReader()
{
  delete[] _fileContent;
}

int FileReader::getPos() const
{
  return _curPos;
}