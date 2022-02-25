//
// Created by 洪陈杰 on 2022/2/18.
//

#include "Stmt.h"

using namespace compilers;
using std::to_string;
using std::string;

Stmt *const Stmt::Null = new Stmt();
Stmt *Stmt::Enclosing = Stmt::Null;

// S -> if(E) S1
void If::gen(int startLabel, int nextLabel) {
  int label = newLabel();
  expr->jumping(0, nextLabel); // 生成 E 的跳转指令
  emitLabel(label); // S1 语句的第一条指令的标号
  stmt->gen(label, nextLabel); // 生成 S1 的指令
}

// S -> if(E) S1 else S2
void Else::gen(int startLabel, int nextLabel) {
  int label1 = newLabel();
  int label2 = newLabel();
  expr->jumping(0, label2);
  emitLabel(label1);
  stmt1->gen(label1, label2);
  emit("goto L" + to_string(nextLabel));
  emitLabel(label2);
  stmt2->gen(label2, nextLabel);
}

// S -> while(E) S1
void While::gen(int startLabel, int nextLabel) {
  next = nextLabel; // 保存 S 的下一条指令标号，用于 break
  expr->jumping(0, nextLabel); // E 的跳转指令
  int label = newLabel();
  emitLabel(label); // S1 的第一条指令的标号
  stmt->gen(label, startLabel);
  emit("goto L" + to_string(startLabel));
}

// S -> do S1 while(E)
void Do::gen(int startLabel, int nextLabel) {
  next = nextLabel; // 保存 S 的下一条指令标号，用于 break
  int label = newLabel();
  stmt->gen(startLabel, label);
  emitLabel(label); // E 的第一条指令的标号
  expr->jumping(startLabel, 0); // E 的跳转指令
}

// S -> id = E 标识符赋值语句
void Set::gen(int startLabel, int nextLabel) {
  emit(id->toString() + " = " + expr->gen()->toString());
}

// S -> id[E1] = E2 数组元素赋值语句
void SetElem::gen(int startLabel, int nextLabel) {
  Expr *eIndex = index->reduce();
  Expr *eExpr = expr->reduce();
  emit(array->toString() + " [ " + eIndex->toString() + " ] = " + eExpr->toString());
}

// S -> S1 S2
void Seq::gen(int startLabel, int nextLabel) {
  // 空语句不产生任何指令，避免使用额外的标号
  if (stmt1 == Stmt::Null) stmt2->gen(startLabel, nextLabel);
  else if (stmt2 == Stmt::Null) stmt1->gen(startLabel, nextLabel);
  else {
    int label = newLabel(); // 新建标号，用在第一条语句的跳转上
    stmt1->gen(startLabel, label);
    emitLabel(label);
    stmt2->gen(label, nextLabel);
  }
}

// S -> break
void Break::gen(int startLabel, int nextLabel) {
  // 跳转到外层结构的下一条指令
  emit("goto L" + to_string(stmt->next));
}