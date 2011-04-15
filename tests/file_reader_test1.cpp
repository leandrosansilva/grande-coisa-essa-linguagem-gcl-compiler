#include <src/lexical/filereader.h>

#include <iostream>

int main(int argc, char **argv)
{
  Lexical::FileReader file(argv[1]);
  
  while (file.canRead()) {
    const char c(file.getChar());
    std::cout << "'" << c << "' => " << file.getLineNumber() << "x" << file.getColumnNumber() 
              << " e coluna anterior: " << file.getPreviousColumnNumber()  << std::endl;
  }
  
  return 0;
}
