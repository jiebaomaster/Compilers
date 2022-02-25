//
// Created by 洪陈杰 on 2022/2/18.
//

#ifndef COMPILER_NODE_H
#define COMPILER_NODE_H

#include "../Lexer.h"

namespace compilers {
  // 抽象语法树的节点
  class Node {
  public:
    Node();

    /* 用于生成三地址代码 */
    // 新建标签，递增全局标签号
    int newLabel();

    // 输出标签 labelIndex 的三地址代码
    void emitLabel(int labelIndex);

    // 输出字符串 s 中的三地址代码
    void emit(const std::string &s);

    // 抛出错误
    void error(const std::string &s);

  private:
    int lexLine; // 本节点对应的构造在源程序的行号
    static int labels; // 全局标签号
  };
}


#endif //COMPILER_NODE_H
