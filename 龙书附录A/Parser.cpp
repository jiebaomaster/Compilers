//
// Created by 洪陈杰 on 2022/2/17.
//

#include "Parser.h"
#include <iostream>

using namespace compilers;
using std::string;
using std::cout;
using std::endl;
using std::to_string;

void Parser::move() {
  lookHead = lexer->scan();
}

void Parser::error(const std::string &s) {
  cout << endl << "line["+ to_string(Lexer::line) +"]: " << s << endl;
  exit(-1);
}

void Parser::match(int tag) {
  if (tag == lookHead->tag) move();
  else error("syntax error");
}

/* 语法制导的翻译 */

// program -> block
void Parser::program() {
  // 分析词法单元，递归向下构建整个抽象语法树
  auto s = block();
  // 遍历抽象语法树，生成三地址代码
  int startLabel = s->newLabel();
  int nextLabel = s->newLabel();
  s->emitLabel(startLabel);
  s->gen(startLabel, nextLabel);
  s->emitLabel(nextLabel);
}

/**
 * 处理语句
 */

// block -> { decls stmts }
Stmt *Parser::block() {
  match('{');
  auto savedEnv = top; // 保存当前作用域
  top = new Env(top); // 新建作用域
  // 标识符定义，只影响符号表，不产生抽象语法树结点
  decls();
  // 语句，产生对应的抽象语法树结点
  auto s = stmts();
  match('}');
  top = savedEnv; // 恢复当前作用域
  return s;
}

// decls -> decls decl
void Parser::decls() {
  while (lookHead->tag == Tag::BASIC) { // decl -> type id;
    auto idType = type(); // 基本类型或数组类型
    auto idToken = lookHead;
    match(Tag::ID);
    match(';');
    // 在作用域的符号表中添加新的符号
    auto id = new Id(idToken, idType, used);
    top->put(idToken, id);
    used += idType->width;
  }
}

// type -> type[num] | basic
Type *Parser::type() {
  Type *t = dynamic_cast<Type *>(lookHead); // 某个基本类型
  match(Tag::BASIC); // type 是一个基本类型
  if (lookHead->tag != '[') return t; // type -> basic
  else return dims(t); // 数组类型，type -> type[num]
}

Type *Parser::dims(Type *elemType) {
  // [arrLen]
  match('[');
  auto arrLen = dynamic_cast<Num *>(lookHead);
  match(Tag::NUM);
  match(']');
  // 多维数组，[arrLen][arrLen]...
  if (lookHead->tag == '[')
    elemType = dims(elemType); // 多维数组的元素类型是数组
  return new Array(arrLen->value, elemType);
}

Stmt *Parser::stmts() {
  if (lookHead->tag == '}') return Stmt::Null;
  else return new Seq(stmt(), stmts());
}

Stmt *Parser::stmt() {
  Expr *x;
  Stmt *s1, *s2;
  Stmt *savedStmt;
  switch (lookHead->tag) {
    case ';': // 空语句
      move();
      return Stmt::Null;
    case Tag::IF: // if
      // if(bool) s1
      match(Tag::IF);
      match('(');
      x = boolean();
      match(')');
      s1 = stmt();
      // 没有接 else，是 if 结点
      if (lookHead->tag != Tag::ELSE)
        return new If(x, s1);
      // 是 if else 结点
      match(Tag::ELSE);
      s2 = stmt();
      return new Else(x, s1, s2);
    case Tag::WHILE: {
      auto whileNode = new While();
      savedStmt = Stmt::Enclosing; // 保存外层语句结点
      Stmt::Enclosing = whileNode; // 当前语句块的外层结点为 whileNode
      // while(bool) s1
      match(Tag::WHILE);
      match('(');
      x = boolean();
      match(')');
      s1 = stmt();
      /**
       * 构建 s1 时可能会有 break，需要依赖外层结点 whileNode，
       * 而初始化 whileNode 又依赖 s1，
       * 所以 whileNode 的构建方式是先构建一个空的，在 s1 构建完成之后再初始化
       * 下面 doNode 的构建方式同理
       */
      whileNode->init(x, s1);
      Stmt::Enclosing = savedStmt; // 恢复外层语句结点
      return whileNode;
    }
    case Tag::DO: {
      auto doNode = new Do();
      savedStmt = Stmt::Enclosing;
      Stmt::Enclosing = doNode;
      // do s1 while(bool);
      match(Tag::DO);
      s1 = stmt();
      match(Tag::WHILE);
      match('(');
      x = boolean();
      match(')');
      match(';');
      doNode->init(x, s1);
      Stmt::Enclosing = savedStmt;
      return doNode;
    }
    case Tag::BREAK:
      // break;
      match(Tag::BREAK);
      match(';');
      return new Break();
    case '{':
      return block();
    default: // 赋值语句
      return assign();
  }
}

// S -> L = bool;
// L -> id | L[bool]
Stmt *Parser::assign() {
  Stmt *s;
  Token *idToken = lookHead;
  match(Tag::ID);
  auto id = top->get(idToken); // 在作用域链中查找标识符
  if (id == nullptr) // 判断标识符是否已定义
    error(idToken->toString() + " undeclared");
  if (lookHead->tag == '=') { // 普通符号的赋值语句，S -> id = bool
    move();
    s = new Set(id, boolean());
  } else { // （多维）数组的赋值语句 S -> L = E
    auto x = offset(id);
    match('=');
    s = new SetElem(x, boolean());
  }
  match(';');
  return s;
}

// I -> [E] | [E] I
Access *Parser::offset(Id *id) {
  Expr* w;
  Expr* t1, *t2;
  Expr* loc;
  // 第一个下标 I -> [E]
  match('[');
  auto index = boolean();
  match(']');
  auto elemType = dynamic_cast<Array*>(id->type)->of;
  w = new Constant(elemType->width);
  t1 = new Arith(new Token('*'), index, w);
  loc = t1;
  // 多维下标，P245 累加每一级的内存占用长度
  while (lookHead->tag == '[') {
    match('[');
    index = boolean();
    match(']');
    elemType = dynamic_cast<Array*>(elemType)->of;
    w = new Constant(elemType->width);
    t1 = new Arith(new Token('*'), index, w); // t1 = index * w
    t2 = new Arith(new Token('+'), loc, t1); // t = loc + t1
    loc = t2;
  }

  return new Access(id, loc, elemType);
}

/**
 * 处理表达式
 */

// bool -> bool || join | join
Expr *Parser::boolean() {
  Expr *x = join();
  while (lookHead->tag == Tag::OR) {
    auto boolToken = lookHead;
    move();
    x = new Or(boolToken, x, join());
  }
  return x;
}

Expr *Parser::join() {
  Expr *x = equality();
  while (lookHead->tag == Tag::AND) {
    auto joinToken = lookHead;
    move();
    x = new And(joinToken, x, equality());
  }
  return x;
}

Expr *Parser::equality() {
  Expr *x = rel();
  while (lookHead->tag == Tag::EQ || lookHead->tag == Tag::NE) {
    auto equality = lookHead;
    move();
    x = new Rel(equality, x, rel());
  }
  return x;
}

Expr *Parser::rel() {
  Expr *x = expr();
  switch (lookHead->tag) {
    case '<':
    case Tag::LE:
    case '>':
    case Tag::GE: {
      auto relToken = lookHead;
      move();
      return new Rel(relToken, x, expr());
    }
    default:
      return x;
  }
}

Expr *Parser::expr() {
  Expr *x = term();
  while (lookHead->tag == '+' || lookHead->tag == '-') {
    auto exprToken = lookHead;
    move();
    x = new Arith(exprToken, x, term());
  }
  return x;
}

Expr *Parser::term() {
  Expr *x = unary();
  while (lookHead->tag == '*' || lookHead->tag == '/') {
    auto termToken = lookHead;
    move();
    x = new Arith(termToken, x, unary());
  }
  return x;
}

Expr *Parser::unary() {
  switch (lookHead->tag) {
    case '!': {
      auto notToken = lookHead;
      move();
      return new Unary(notToken, unary());
    }
    case '-':
      move();
      return new Not(Word::minus_, unary());
    default:
      return factor();
  }
}

Expr *Parser::factor() {
  Expr *x = nullptr;
  switch (lookHead->tag) {
    case '(':
      move();
      x = boolean();
      match(')');
      return x;
    case Tag::NUM:
      x = new Constant(lookHead, Type::int_);
      move();
      return x;
    case Tag::REAL:
      x = new Constant(lookHead, Type::float_);
      move();
      return x;
    case Tag::TRUE:
      x = Constant::True;
      move();
      return x;
    case Tag::FALSE:
      x = Constant::False;
      move();
      return x;
    case Tag::ID: {
      auto id = top->get(lookHead); // 在作用域链中查找标识符
      if (id == nullptr) // 判断标识符是否已定义
        error(lookHead->toString() + " undeclared");
      move();
      if (lookHead->tag != '[') return id;
      else return offset(id);
    }
    default:
      error("syntax error");
      return x;
  }
}