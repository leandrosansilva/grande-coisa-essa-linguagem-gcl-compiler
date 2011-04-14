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


#ifndef LEXICAL_ANALYSER_H
#define LEXICAL_ANALYSER_H

#include "filereader.h"

#include <common/token.h>

namespace Lexical {

class Analyser
{
public:
  /* chamo o analizador passando qual arquivo deve fazer parser */
  Analyser(FileReader &file);
  
  /* me retorna se posso continuar a pegar token ou se já terminou */
  bool canReadToken() const;
  
  /* Pega o próximo token */
  Common::Token getToken();
  
private:
  FileReader &_file;
};

}

#endif // LEXICAL_ANALYSER_H
