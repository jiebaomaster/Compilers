//
// Created by 洪陈杰 on 2022/2/15.
//

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <string>
#include <unordered_map>

namespace compilers {
  class Token;

  class Word;

  /**
   * 词法分析器
   * 扫描源代码，返回词法单元
   */
  class Lexer {
  public:
    Lexer();

    // 扫描源代码字符串，构造相应的词法单元返回
    Token *scan();

    static int line; // 当前扫描的代码行数
  private:
    void reserve(Word *pWord);

    // 从流中读入一个字符到peek
    int readChar();

    // 从流中读取一个字符，判读值是否为 c，读取流标后移一位
    bool readChar(char c);

    // 将 peek 放回输入流中，有些词法单元需要预读字符才能判断，预读的字符需要放回
    void putBack();

    char peek; // 当前处理的字符

    // 词素字符串=》词法单元
    std::unordered_map<std::string, Word *> words;
  };
}


#endif //COMPILER_LEXER_H
