//
// Created by 洪陈杰 on 2022/2/17.
//

#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include "../Token.h"

namespace compilers {
  // 基本数据类型定义的词法单元
  class Type : public Word {
  public:
    Type(const std::string &lexeme, int tag, int w)
            : Word(lexeme, tag), width(w) {}

    /* 用于类型转换 */

    // 判断 t 是否是数字类型（int、float、char），数字类型之间可以相互转换
    static bool numeric(Type *t);

    // 当一个算数运算符运用在两个数字类型上时，结果类型是两个类型中的更大的那个
    static Type *max(Type *t1, Type *t2);

    // 预先定义的基本类型
    static Type *const int_, *const float_, *const char_, *const bool_;
    int width; // 类型占用的存储空间，单位字节
  };

  // 数组类型定义的词法单元
  class Array : public Type {
  public:
    /**
     * width 表示数组的总存储空间占用，等于元素个数*元素类型长度
     * @param sz 元素个数
     * @param t 数组的元素类型，多维数组的元素类型是子数组
     */
    Array(int sz, Type *t)
            : Type("[]", Tag::INDEX, sz * t->width),
              size(sz),
              of(t) {}

    std::string toString() const override {
      return "[" + std::to_string(size) + "]" + of->toString();
    }

    int size; // 数组元素个数
    Type *of; // 数组元素类型
  };
}


#endif //COMPILER_TYPE_H
