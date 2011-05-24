#ifndef COMMONSTRING
#define COMMONSTRING

#include <string>
#include <iostream>

namespace Common {
  
/* Um tipo especial de string, que possui tbm o operador -,
 * que remove da string os caracteres subtraídos 
*/
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
    const int s(size());
    
    /* sei lá porque razão, mas é melhor trabalhar com memória */
    const char *d(data());
    
    for (int i(0); i < s; i++) {
      if (!other.hasChar(d[i])) final += d[i];
    }
    
    return final;
  }
  
  /* A string possui o caractere passado? */
  virtual bool hasChar(char c) const
  {
    return find(c) != npos;
  }
};

}
#endif
