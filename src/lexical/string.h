#ifndef LEXICALSTRING
#define LEXICALSTRING

#include <string>

namespace Lexical {
class String: public std::string
{
public:
  String(const char *str): std::string(str)
  {
  }
  
  String(const String &other): std::string(other)
  {
  }
  
  String(std::basic_string<char,std::char_traits<char>,std::allocator<char> > other): std::string(other)
  {
  }
  
  String(): std::string() 
  {
  }
  
  const String operator+(const String &o1)
  {
  }
  
  bool hasChar(char c)
  {
    return find(c) != npos;
  }
};
 
}
#endif