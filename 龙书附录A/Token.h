//
// Created by 洪陈杰 on 2022/2/18.
//

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include <string>

namespace compilers {
  /**
   * 词法单元
   */
  class Token {
  public:
    explicit Token(int t) : tag(t) {
    }

    virtual std::string toString() const {
      return {static_cast<char>(tag)};
    }

    const int tag; // 词法单元类型
  };

  /**
   * 词法单元类型
   * 0～255 用来表示单字符词法单元，如 +、*
   */
  enum Tag {
    AND = 256, // 关系运算符 &&
    BASIC = 257, // 基础变量类型的关键字
    BREAK = 258, // 关键字 break
    DO = 259, // 关键字 do
    ELSE = 260, // 关键字 else
    EQ = 261, // 关系运算符 ==
    FALSE = 262, // 关键字 false
    GE = 263, // 关系运算符 >=
    ID = 264, // 标志符
    IF = 265, // 关键字 if
    INDEX = 266, // 数组类型
    LE = 267, // 关系运算法 <=
    MINUS = 268, // 单目减号
    NE = 269, // 关系运算符 !=
    NUM = 270, // 整数常量
    OR = 271, // 关系运算符 ||
    REAL = 272, // 浮点数常量
    TEMP = 273,
    TRUE = 274, // 关键字 true
    WHILE = 275, // 关键字 while
  };

  /**
   * 整数常量词法单元
   */
  class Num : public Token {
  public:
    explicit Num(int v) : Token(Tag::NUM), value(v) {
    }

    std::string toString() const override {
      return std::to_string(value);
    }

    const int value; // 整数值
  };

  /**
   * 浮点数词法单元
   */
  class Real : public Token {
  public:
    explicit Real(double v) : Token(Tag::REAL), value(v) {}

    std::string toString() const override {
      return std::to_string(value);
    }

    const double value; // 浮点数值
  };

  /**
   * 标志符/关键字/多字符运算的词法单元
   */
  class Word : public Token {
  public:
    /**
     * @param tag 词法单元类型
     * @param lexeme 词素字符串
     */
    Word(const std::string &lexeme, int tag) : Token(tag), lexeme(lexeme) {
    }

    Word(std::string &&lexeme, int tag) : Token(tag), lexeme(std::move(lexeme)) {}

    std::string toString() const override {
      return lexeme;
    }

    const std::string lexeme; // 词素字符串
    // 预先定义的词法单元
    static Word *const and_, *const or_, *const eq_,
            *const ne_, *const le_, *const ge_,
            *const minus_, *const true_, *const false_, *const temp_;
  };

}

#endif //COMPILER_TOKEN_H
