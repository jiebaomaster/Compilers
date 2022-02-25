//
// Created by 洪陈杰 on 2022/2/17.
//

#include "Type.h"

using namespace compilers;

Type *const Type::int_ = new Type("int", Tag::BASIC, 4);
Type *const Type::float_ = new Type("float", Tag::BASIC, 8);
Type *const Type::char_ = new Type("char", Tag::BASIC, 1);
Type *const Type::bool_ = new Type("bool", Tag::BASIC, 1);

bool Type::numeric(Type *t) {
  return t == Type::int_ || t == Type::float_ || t == Type::char_;
}

Type *Type::max(Type *t1, Type *t2) {
  if (!numeric(t1) || !numeric(t2)) return nullptr;
  else if (t1 == float_ || t2 == float_) return float_;
  else if (t1 == int_ || t2 == int_) return int_;
  else return char_;
}
