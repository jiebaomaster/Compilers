//
// Created by 洪陈杰 on 2022/2/17.
//

#include "Parser.h"
#include <iostream>

using namespace compilers;
using namespace std;

int main() {
  auto lexer = new Lexer();
  auto parser = new Parser(lexer);
  parser->program();
  cout << endl;
}