//
// Created by 洪陈杰 on 2022/2/15.
//

#include "Lexer.h"
#include "Token.h"
#include "./symbol/Type.h"
#include <iostream>
#include <cctype>

using namespace compilers;

using std::cin;
using std::string;

int Lexer::line = 1;

void Lexer::reserve(Word *pWord) {
  words.insert({pWord->lexeme, pWord});
}

Lexer::Lexer() : peek(' ') {
  // 在字符串表中初始化关键字的词法单元映射，使得在识别词素时可以区分保留字和标识符
  reserve(new Word("if", Tag::IF));
  reserve(new Word("else", Tag::ELSE));
  reserve(new Word("while", Tag::WHILE));
  reserve(new Word("do", Tag::DO));
  reserve(new Word("break", Tag::BREAK));
  reserve(Word::true_);
  reserve(Word::false_);
  reserve(Type::int_);
  reserve(Type::float_);
  reserve(Type::char_);
  reserve(Type::bool_);
}

Token *Lexer::scan() {
  /* 跳过空白符 */
  while (readChar()) {
    if (peek == ' ' || peek == '\t') {
      continue;
    } else if (peek == '\n') {
      line++;
    } else {
      break;
    }
  }

  /* 处理需要预读字符才能判断的词法单元 */
  switch (peek) {
    case '&':
      if (readChar('&')) return Word::and_;
      else {
        putBack();
        return new Token('&');
      }
    case '|':
      if (readChar('|')) return Word::or_;
      else {
        putBack();
        return new Token('|');
      }
    case '=':
      if (readChar('=')) return Word::eq_;
      else {
        putBack();
        return new Token('=');
      }
    case '!':
      if (readChar('=')) return Word::ne_;
      else {
        putBack();
        return new Token('!');
      }
    case '<':
      if (readChar('=')) return Word::le_;
      else {
        putBack();
        return new Token('<');
      }
    case '>':
      if (readChar('=')) return Word::ge_;
      else {
        putBack();
        return new Token('>');
      }
  }

  /* 处理数字 */
  if (isdigit(peek)) {
    int v = 0;
    do { // 处理整数部分
      v = v * 10 + peek - '0';
      readChar();
    } while (isdigit(peek));
    if (peek != '.') { // 下一个不是小数点，说明是整数词法单元
      putBack();
      return new Num(v);
    }
    // 处理浮点数
    double x = v; // 最终浮点数结果
    int d = 10; // 精度
    while (true) { // 处理小数部分
      readChar();
      if (!isdigit(peek)) {
        putBack();
        break;
      }
      x += static_cast<double>(peek - '0') / d;
      d *= 10;
    }
    return new Real(x);
  }

  /* 处理保留字和标识符 */
  if (isalpha(peek)) {
    string tokenBuf;
    do {
      tokenBuf.push_back(peek);
      readChar();
    } while (isdigit(peek) || isalpha(peek));
    putBack();
    // 已存在的标识符直接返回，可能是保留字或已识别的标识符
    if (words.find(tokenBuf) != words.end()) {
      return words[tokenBuf];
    }
    // 新建一个标识符的词法单元
    auto w = new Word(tokenBuf, Tag::ID);
    words[w->lexeme] = w;
    return w;
  }
  // 处理一般单字符词法单元，任何peek中的字符都应该被作为词法单元返回
  auto token = new Token(peek);
  peek = ' ';
  return token;
}

int Lexer::readChar() {
  cin.get(peek);
  return cin.gcount();
}

bool Lexer::readChar(char c) {
  readChar();
  if (peek != c) return false;
  peek = ' ';
  return true;
}

void Lexer::putBack() {
  cin.putback(peek);
}