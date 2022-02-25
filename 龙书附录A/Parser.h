//
// Created by 洪陈杰 on 2022/2/17.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Lexer.h"
#include "Token.h"
#include "symbol/Env.h"
#include "astNode/Expr.h"
#include "astNode/Stmt.h"
#include <string>

namespace compilers {
  class Lexer;

  class Parser {
  public:
    explicit Parser(Lexer *lexer)
            : lexer(lexer), lookHead(nullptr), top(nullptr), used(0) {
      move();
    }

    /* 语法制导的翻译，每个非终结符对应一个过程 */
    // 翻译开始，program -> block
    void program();

  private:
    /**
     * 处理语句
     */

    // block -> { decls stmts }
    Stmt *block();

    // decls -> decls decl
    // decl -> type id;
    void decls();

    // type -> type[num] | basic
    Type *type();
    // 辅助函数，处理词法单元，返回（多维）数组的类型
    Type *dims(Type *elemType);

    // stmts -> stmts stmt
    Stmt *stmts();

    Stmt *stmt();
    // stmt -> loc = bool;
    // loc -> id | loc[bool]
    Stmt* assign();
    /**
     * 辅助函数，为数组访问的地址计算生成抽象语法树结点
     * @param Id* 某个类型是数组的标识符
     */
    Access* offset(Id* id);

    /**
     * 处理表达式
     */

    // 布尔表达式 bool -> bool || join | join
    Expr* boolean();
    // join -> join && equality | equality
    Expr* join();
    // equality -> equality == rel | equality != rel | rel
    Expr* equality();
    // rel -> expr < expr | expr <= expr | expr > expr | expr >= expr | expr
    Expr* rel();
    // expr -> expr + term | expr - term | term
    Expr* expr();
    // term -> term * unary | term / unary | unary
    Expr* term();
    // unary -> ! unary | - unary | factor
    Expr* unary();
    // factor -> (bool) | loc | num | real | true | false
    Expr* factor();

    /**
     * 辅助函数
     */

    // 从词法分析器中读取一个词法单元
    void move();

    // 报错
    void error(const std::string &s);

    // 如果当前 lookHead 的类型与 tag 相匹配就向前读一个词法单元
    void match(int tag);

    Lexer *lexer; // 这个语法分析器的词法分析器
    Token *lookHead; // 向前看词法单元
    Env *top; // 当前或者顶层的符号表
    int used; // 用于变量声明的存储位置
  };
}

#endif //COMPILER_PARSER_H
