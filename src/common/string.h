#ifndef COMMONSTRING
#define COMMONSTRING

#include <string>
#include <iostream>

namespace Common {
class String: public std::string
{
public:
  String(const char *str): std::string(str)
  {
  }
  
  String(const String &other): std::string(other)
  {
  }
  
  String(const std::string &other): std::string(other)
  {
  }
  
  String(): std::string() 
  {
  }
  
  /* remove da string os caracteres passados.
   * Atua como "complementar" 
  */
  const String operator-(const String &other) const
  {
    String final;
    
    for (const_iterator i = begin(); i != end(); i++) {
      if (!other.hasChar(*i)) final += *i;
    }
    
    return final;
  }
  
  virtual bool hasChar(char c) const
  {
    //std::cout << "Buscando '" << c << "' em '" << *this << "'" << std::endl;
    return find(c) != npos;
  }
};

}
#endif
