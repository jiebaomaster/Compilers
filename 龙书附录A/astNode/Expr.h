//
// Created by 洪陈杰 on 2022/2/18.
//

#ifndef COMPILER_EXPR_H
#define COMPILER_EXPR_H

#include "Node.h"
#include "../symbol/Type.h"

namespace compilers {
  // 抽象语法树中的"表达式"结点基类
  class Expr : public Node {
  public:
    Expr(Token *token, Type *type)
            : op(token), type(type) {}

    // 返回一个对应于当前结点且可以作为三地址指令右部的项
    // 即返回的新表达式只包含直接表达式，子表达式不包含子表达式
    // 通常需要对子表达式进行规约，即生成子表达式的代码
    virtual Expr *gen() { return this; }

    // 把表达式计算（归约）为一个单一的地址，应该返回一个常量 或者 一个标识符 或者 一个临时名字
    //
    virtual Expr *reduce() { return this; }

    /* 为逻辑表达式生成跳转代码 P261 图6-39*/
    virtual void jumping(int trueLabel, int falseLabel) {
      emitJumps(toString(), trueLabel, falseLabel);
    }

    /**
     * 输出跳转的三地址代码
     * @param test 待判断的逻辑表达式
     * @param trueLabel test 为 true 时跳转的标签
     * @param falseLabel test 为 false 时跳转的标签
     */
    void emitJumps(const std::string &test, int trueLabel, int falseLabel);

    // 返回可以表示本节点操作的字符串
    virtual std::string toString() const { return op->toString(); }

    Token *op; // 表达式结点的运算符词法单元，即做哪种运算
    Type *type; // 表达式结点的类型，即运算结果的类型
  };

  /**
   * 抽象语法树中的叶子结点，包括标识符、临时变量、常量
   */

  // 抽象语法树中的"标识符"结点，肯定是叶子结点
  class Id : public Expr {
  public:
    Id(Token *token, Type *type, int offset)
            : Expr(token, type), offset(offset) {}

    int offset; // 标识符的相对地址
  };

  // 抽象语法树中的"临时变量"结点
  class Temp : public Expr {
  public:
    explicit Temp(Type *type) : Expr(Word::temp_, type) {
      number = ++count;
    }

    std::string toString() const override {
      return "t" + std::to_string(number);
    }

  private:
    static int count; // 全局临时变量计数器
    int number; // 本实例是第几个临时变量
  };

  // 抽象语法树中的"常量"结点，
  class Constant : public Expr {
  public:
    Constant(Token *token, Type *type) : Expr(token, type) {}

    // 构造整数常量
    explicit Constant(int i) : Expr(new Num(i), Type::int_) {}

    void jumping(int trueLabel, int falseLabel) override;

    static Constant *const True, *const False;
  };


  /**
   * 抽象语法树中的"算数运算符"结点，包括单目运算符、双目运算符、数组访问运算符
   */

  // 抽象语法树中的"算数运算符"结点的基类
  class Op : public Expr {
  public:
    Op(Token *token, Type *type)
            : Expr(token, type) {}

    // 对算数表达式进行归约将产生新的临时变量，并生成三地址代码
    // 先调用gen() 生成子表达式的代码，再使用临时变量保存表达式的值
    Expr *reduce() override {
      Expr *x = gen(); // 生成子表达式的代码
      Temp *t = new Temp(type); // 创建临时变量存储算数表达式的值
      emit(t->toString() + " = " + x->toString()); // t = op
      return t;
    }
  };

  // 抽象语法树中的"双目运算符"结点
  class Arith : public Op {
  public:
    /**
     * @param token 运算符的词法单元
     * @param lExpr 左子表达式
     * @param rExpr 右子表达式
     */
    Arith(Token *token, Expr *lExpr, Expr *rExpr)
            : Op(token, nullptr), lExpr(lExpr), rExpr(rExpr) {
      // 双目运算符结点的类型是两个子表达式类型中的更大的那个
      type = Type::max(lExpr->type, rExpr->type);
      if (type == nullptr) error("type error");
    }

    Expr *gen() override;

    std::string toString() const override {
      return lExpr->toString() + " " + op->toString() + " " + rExpr->toString();
    }

    Expr *lExpr; // 左子表达式
    Expr *rExpr; // 右子表达式
  };

  // 抽象语法树中的"单目运算符"结点
  class Unary : public Op {
  public:
    Unary(Token *token, Expr *expr)
            : Op(token, nullptr), expr(expr) {
      type = Type::max(Type::int_, expr->type);
    }

    Expr *gen() override;

    std::string toString() const override {
      return op->toString() + " " + expr->toString();
    }

    Expr *expr; // 子表达式
  };

  // 抽象语法树中的"数组元素访问"结点
  class Access : public Op {
  public:
    /**
     * @param arr 数组名标识符
     * @param index 数组索引表达式
     * @param type 数组元素类型
     */
    Access(Id *arr, Expr *index, Type *type)
            : Op(new Word("[]", Tag::INDEX), type),
              array(arr), index(index) {}

    Expr *gen() override;

    // 布尔表达式的可能是一个数组访问，如 if(arr[0])
    // 生成使用数组元素访问进行判断的跳转代码
    void jumping(int trueLabel, int falseLabel) override;

    std::string toString() const override {
      return array->toString() + "[" + index->toString() + "]";
    }

    Id *array; // 数组名标识符
    Expr *index; // 数组索引表达式
  };

  /**
   * P260
   * 抽象语法树中的"逻辑运算符"结点，与、或、非，和关系运算符
   */

  // 抽象语法树中的"逻辑运算符"结点基类
  class Logical : public Expr {
  public:
    Logical(Token *token, Expr *lExpr, Expr *rExpr, bool needCheck = true) :
            Expr(token, nullptr), lExpr(lExpr), rExpr(rExpr) {
      if (needCheck) {
        type = check(lExpr->type, rExpr->type);
        if (type == nullptr) error("type error");
      }
    }

    // 运算分量类型检查
    // 与、或、非 运算要求两个分量都是 bool 类型
    Type *check(Type *t1, Type *t2) {
      if (t1 == Type::bool_ && t2 == Type::bool_) return Type::bool_;
      else return nullptr;
    }

    Expr *gen() override;

    std::string toString() const override {
      return lExpr->toString() + " " + op->toString() + " " + rExpr->toString();
    }

    // 逻辑运算符的两个运算分量表达式
    Expr *lExpr;
    Expr *rExpr;
  };

  // B -> B1 || B2
  class Or : public Logical {
  public:
    Or(Token *token, Expr *lExpr, Expr *rExpr) : Logical(token, lExpr, rExpr) {}

    void jumping(int trueLabel, int falseLabel) override;
  };

  // B -> B1 && B2
  class And : public Logical {
  public:
    And(Token *token, Expr *lExpr, Expr *rExpr) : Logical(token, lExpr, rExpr) {}

    void jumping(int trueLabel, int falseLabel) override;
  };

  // B -> !B1
  class Not : public Logical {
  public:
    Not(Token *token, Expr *expr) : Logical(token, expr, expr) {}

    void jumping(int trueLabel, int falseLabel) override;

    std::string toString() const override {
      return op->toString() + " " + rExpr->toString();
    }
  };

  // B -> B1 rel B2
  class Rel : public Logical {
  public:
    Rel(Token *token, Expr *lExpr, Expr *rExpr)
            : Logical(token, lExpr, rExpr, false) {
      type = check(lExpr->type, rExpr->type);
      if (type == nullptr) error("type error");
    }

    // 运算分量类型检查
    // 两个运算分量的类型必须相等，且不能为数组类型
    Type *check(Type *t1, Type *t2) {
      if (t1->tag == Tag::INDEX || t2->tag == Tag::INDEX) return nullptr;
      else if (t1 == t2) return Type::bool_;
      else return nullptr;
    }

    void jumping(int trueLabel, int falseLabel) override;
  };


}


#endif //COMPILER_EXPR_H
