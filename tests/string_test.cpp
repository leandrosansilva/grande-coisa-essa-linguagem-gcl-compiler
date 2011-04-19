#include <common/string.h>

#include <iostream>

int main(int argc, char **argv)
{
  using namespace Common;
  
  const String alpha("aeiou");
  const String digit("0123456789");
  
  const String any(alpha + digit);
  
  const String pair("02468");
  const String impar("13579");
  
  const String r(any - pair);
  const String s(any - impar);
  
  std::cout << alpha << std::endl;
  std::cout << digit << std::endl;
  std::cout << any << std::endl;
  std::cout << r << std::endl;
  std::cout << s << std::endl;
  
  return 0;
}

