//
// Created by 洪陈杰 on 2022/2/18.
//

#ifndef COMPILER_STMT_H
#define COMPILER_STMT_H

#include "Node.h"
#include "Expr.h"

namespace compilers {
  // 抽象语法树中的"语句"结点基类
  class Stmt : public Node {
  public:
    Stmt() : next(-1) {}

    /**
     * 生成语句的三地址代码
     * @param startLabel 语句的第一条指令的标号
     * @param nextLabel 这个语句的所有指令之后的第一条指令的标号
     */
    virtual void gen(int startLabel, int nextLabel) {}

    static Stmt *const Null; // 空语句
    static Stmt *Enclosing;
    int next; // 保存这个语句的所有指令之后的第一条指令的标号
  };

  // S -> if(E) S1
  class If : public Stmt {
  public:
    If(Expr *e, Stmt *s1) : expr(e), stmt(s1) {
      if (expr->type != Type::bool_)
        expr->error("boolean required in if");
    }

    void gen(int startLabel, int nextLabel) override;

    Expr *expr; // 判断表达式
    Stmt *stmt; // E 为真时执行的语句
  };

  // S -> if(E) S1 else S2
  class Else : public Stmt {
  public:
    Else(Expr *e, Stmt *s1, Stmt *s2) : expr(e), stmt1(s1), stmt2(s2) {
      if (expr->type != Type::bool_)
        expr->error("boolean required in if else");
    }

    void gen(int startLabel, int nextLabel) override;

    Expr *expr; // 判断表达式
    Stmt *stmt1; // E 为真时执行的语句
    Stmt *stmt2; // E 为假时执行的语句
  };

  // S -> while(E) S1
  class While : public Stmt {
  public:
    // TODO 对 while 的构建有两个部分 While() 和 init()
    While() : expr(nullptr), stmt(nullptr) {}
    void init(Expr* e, Stmt* s) {
      expr = e;
      stmt = s;
      if (e->type != Type::bool_)
        error("boolean required in while");
    }
    void gen(int startLabel, int nextLabel) override;

    Expr* expr;
    Stmt* stmt;
  };


  // S -> do S1 while(E)
  class Do : public Stmt {
  public:
    // TODO 对 Do 的构建有两个部分 While() 和 init()
    Do() : expr(nullptr), stmt(nullptr) {}
    void init(Expr* e, Stmt* s) {
      expr = e;
      stmt = s;
      if (e->type!= Type::bool_)
        error("boolean required in do");
    }
    void gen(int startLabel, int nextLabel) override;

    Expr* expr;
    Stmt* stmt;
  };

  // S -> id = E 标识符赋值语句
  class Set : public Stmt {
  public:
    Set(Id *id, Expr *expr) : id(id), expr(expr) {
      if (check(id->type, expr->type) == nullptr)
        error("type error");
    }

    const Type *check( Type *t1,  Type *t2) {
      if (Type::numeric(t1) && Type::numeric(t2)) return t2;
      else if (t1 == Type::bool_ && t2 == Type::bool_) return t2;
      else return nullptr;
    }

    void gen(int startLabel, int nextLabel) override;

    Id *id; // 标识符
    Expr *expr; // 值表达式
  };

  // S -> id[E1] = E2 数组元素赋值语句
  class SetElem : public Stmt {
  public:
    SetElem(Access* access, Expr *expr)
            : array(access->array), index(access->index), expr(expr) {
      if (check(access->type, expr->type) == nullptr)
        error("type error");
    }

    // 类型检查
    const Type *check( Type *t1,  Type *t2) {
      if (t1->tag == Tag::INDEX || t2->tag == Tag::INDEX) return nullptr;
      else if (t1 == t2) return t2;
      else if(Type::numeric(t1) && Type::numeric(t2)) return t2; // 数字类型转换
      else return nullptr;
    }

    void gen(int startLabel, int nextLabel) override;

    Id *array; // 数组名标识符
    Expr *index; // 数组索引表达式
    Expr *expr; // 值表达式
  };

  // S -> S1 S2 语句序列
  class Seq : public Stmt {
  public:
    Seq(Stmt *s1, Stmt *s2) : stmt1(s1), stmt2(s2) {}

    void gen(int startLabel, int nextLabel) override;


    Stmt *stmt1;
    Stmt *stmt2;
  };

  // S -> break
  class Break : public Stmt {
  public:
    Break() {
      if (Stmt::Enclosing == Stmt::Null)
        error("unenclosed break");
      stmt = Stmt::Enclosing;
    }

    void gen(int startLabel, int nextLabel) override;

    Stmt *stmt; // Break 语句的外围构造的语法树结点
  };
}


#endif //COMPILER_STMT_H
