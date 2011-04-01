#include <src/lexical/filereader.h>

#include <iostream>

int main(int argc, char **argv)
{
  std::string a("b.gcl");
  
  Lexical::FileReader file(argv[1]);
  //file.disableIgnoreBlank();
  
  while (file.canRead()) {
    const char c(file.getChar());
    std::cout << c << " => " << file.getLineNumber() << "x" << file.getColumnNumber() << std::endl;
  }
  
  return 0;
}
