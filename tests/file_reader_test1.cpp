#include <src/lexical/filereader.h>

#include <iostream>

int main(int argc, char **argv)
{
  Lexical::FileReader file(argv[1]);
  
  bool back(false);
  
  while (file.canRead()) {
    const char c(file.getChar());
    std::cout << "'" << c << "' => " 
              << file.getLineNumber() 
              << "x"  << file.getColumnNumber() << std::endl;
              
    if (c == 'a') {
      file.back(2);
      back = true;
    } else {
      back = false;
    }
  }
  
  return 0;
}
