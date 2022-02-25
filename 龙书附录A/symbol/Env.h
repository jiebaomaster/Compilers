//
// Created by 洪陈杰 on 2022/2/22.
//

#ifndef COMPILER_ENV_H
#define COMPILER_ENV_H

#include <unordered_map>

namespace compilers {
  class Id;
  class Token;

  // 作用域
  class Env {
  public:
    Env() : prev(nullptr) {}
    /**
     * 在外层作用域中构造内层作用域
     * @param prev
     */
    explicit Env(Env* prev) : prev(prev) {}
    /**
     * 向作用域中添加标识符，声明
     */
    void put(Token* , Id*);
    /**
     * 沿着作用域链查找标识符
     * @param token 词法单元
     * @return 最近的作用域内的标识符
     */
    Id* get(Token* token);
  protected:
    Env* prev; // 作用域链的上一级，即外层作用域
  private:
    std::unordered_map<Token*, Id*> table; // 符号表
  };
}



#endif //COMPILER_ENV_H
