#include <common/string.h>

#include <iostream>

int main(int argc, char **argv)
{
  using namespace Common;
  
  const String alpha("aeiou");
  const String digit("0123456789");
  
  const String any(alpha + digit);
  
  const String pair("02468");
  
  const String r(String(digit + alpha) - pair);
  
  std::cout << alpha << std::endl;
  std::cout << digit << std::endl;
  std::cout << any << std::endl;
  std::cout << r << std::endl;
  
  return 0;
}

