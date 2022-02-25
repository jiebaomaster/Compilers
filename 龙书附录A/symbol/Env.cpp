//
// Created by 洪陈杰 on 2022/2/22.
//

#include "Env.h"

using namespace compilers;

void Env::put(Token *token, Id *id) {
  table.insert({token, id});
}

Id *Env::get(Token *token) {
  Id *target = nullptr;
  for (auto e = this; e != nullptr; e = e->prev) {
    if (e->table.find(token) != e->table.end()) {
      target = e->table[token];
      break;
    }
  }
  return target;
}