//
// Created by 洪陈杰 on 2022/2/18.
//

#include "Expr.h"

using namespace compilers;
using std::string;
using std::to_string;

void Expr::emitJumps(const string &test, int trueLabel, int falseLabel) {
  if (trueLabel != 0 && falseLabel != 0) { // 生成两个逻辑入口的跳转
    emit("if " + test + " goto L" + to_string(trueLabel));
    emit("goto L" + to_string(falseLabel));
  } else if (trueLabel != 0) // 只生成 test==true 时跳转到 t
    emit("if " + test + " goto L" + to_string(trueLabel));
  else if (falseLabel != 0) // 只生成 test==false 时跳转到 f
    emit("ifFalse " + test + " goto L" + to_string(falseLabel));
  else;
}

int Temp::count = 0;

Constant *const Constant::True = new Constant(Word::true_, Type::bool_);
Constant *const Constant::False = new Constant(Word::false_, Type::bool_);

void Constant::jumping(int trueLabel, int falseLabel) {
  if (this == True && trueLabel != 0) emit("goto L" + to_string(trueLabel));
  else if (this == False && falseLabel != 0) emit("goto L" + to_string(falseLabel));
}


Expr *Arith::gen() {
  // 对两个子表达式进行规约，生成子表达式的三地址代码，并返回 不包含子表达式的 新的双目运算符结点
  return new Arith(op, lExpr->reduce(), rExpr->reduce());
}

Expr *Unary::gen() {
  return new Unary(op, expr->reduce());
}

Expr * Access::gen() {
  return new Access(array, index->reduce(), type);
}

void Access::jumping(int trueLabel, int falseLabel) {
  Expr *t = reduce(); // 生成子表达式的代码
  emitJumps(t->toString(), trueLabel, falseLabel); // if t goto Lt
}

Expr *Logical::gen() {
  int f = newLabel(); // 逻辑表达式值为 false 时跳转的标签
  int a = newLabel(); // 该逻辑表达式的三地址代码的下一行代码的标签
  Temp *t = new Temp(type); // 新建临时变量保存逻辑表达式的值
  this->jumping(0, f);       // ifFalse B goto Lf
  emit(t->toString() + " = true"); // t=true
  emit("goto L" + to_string(a));   // goto La
  emitLabel(f);                       // Lf:
  emit(t->toString() + " = false");// t=false
  emitLabel(a);                       // La:
  return t;
}

/**
 * B -> B1 || B2
 *
 * B1.true = if B.true!=fall than B.true else newLabel()
 * B1.false = 0
 * B2.true = B.true
 * B2.false = B.false
 * B.code = if B.true!=fall than B1.code || B2.code
 *          else B1.code || B2.code || label(B1.true)
 */
void Or::jumping(int trueLabel, int falseLabel) {
  int label = trueLabel != 0 ? trueLabel : newLabel();
  // 生成两个子表达式的跳转代码
  lExpr->jumping(label, 0);
  rExpr->jumping(trueLabel, falseLabel);
  if (trueLabel == 0) emitLabel(label);
}

/**
 * B -> B1 && B2
 *
 * B1.true = 0
 * B1.false = if B.false!=fall than B.false else newLabel()
 * B2.true = B.true
 * B2.false = B.false
 * B.code = if B.false!=fall than B1.code || B2.code
 *          else B1.code || B2.code || label(B1.false)
 */
void And::jumping(int trueLabel, int falseLabel) {
  int label = falseLabel != 0 ? falseLabel : newLabel();
  // 生成两个子表达式的跳转代码
  lExpr->jumping(0, label);
  rExpr->jumping(trueLabel, falseLabel);
  if (falseLabel == 0) emitLabel(label);
}

/**
 * B -> !B1
 *
 * B1.true = B.false
 * B1.false = B.true
 * 生成代码时，只需要将 B 的 true 和 false 出口对调，即 B1.jump(f,t)
 */
void Not::jumping(int trueLabel, int falseLabel) {
  rExpr->jumping(falseLabel, trueLabel);
}

void Rel::jumping(int trueLabel, int falseLabel) {
  Expr *a = lExpr->reduce();
  Expr *b = rExpr->reduce();
  string test = a->toString() + " " + op->toString() + " " + b->toString();
  emitJumps(test, trueLabel, falseLabel);
}