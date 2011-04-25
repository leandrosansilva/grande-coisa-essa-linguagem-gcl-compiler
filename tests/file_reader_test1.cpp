#include <src/lexical/filereader.h>

#include <iostream>

int main(int argc, char **argv)
{
  Lexical::FileReader file(argv[1]);
  
  while (file.canRead()) {
    const char c(file.getChar());
    std::cout << "'" << c << "' => " << file.getLineNumber() << std::endl;
  }
  
  return 0;
}
