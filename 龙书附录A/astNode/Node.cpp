//
// Created by 洪陈杰 on 2022/2/18.
//

#include "Node.h"
#include <iostream>

using namespace compilers;
using std::cout;
using std::string;
using std::endl;

int Node::labels = 0;

Node::Node() : lexLine(Lexer::line){

}

int Node::newLabel() {
  return ++labels;
}

void Node::emitLabel(int labelIndex) {
  cout << 'L' << labelIndex << ':';
}

void Node::emit(const string &s) {
  cout << '\t' << s << endl;
}

void Node::error(const string &s) {
  cout << endl << s << endl;
  exit(-1);
}