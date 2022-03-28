#include <iostream>
#include <string_view>


/*
 * Compiled for windows with:
 * clang++.exe -std=c++17 hello.cpp -o hello.exe -O0
 * 
 *  clang version 13.0.0
 *  Target: x86_64-pc-windows-msvc
 *  Thread model: posix
 *
 *
 *  Compiled for linux with:
 *  clang++-12 -std=c++17 hello.cpp -o hello_elfi -O0
 *
 * 	Ubuntu clang version 12.0.0-3ubuntu1~20.04.4
 *	Target: x86_64-pc-linux-gnu
 *	Thread model: posix 
 *  
 */

using namespace std::literals;

constexpr auto hello() -> std::string_view {
  return "Hello World!"sv;
} 

int main() {

  std::cout << hello();  

  return EXIT_SUCCESS;
}
