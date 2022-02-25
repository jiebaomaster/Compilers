//
// Created by 洪陈杰 on 2022/2/18.
//

#include "Token.h"

using namespace compilers;

Word *const Word::and_ = new Word("&&", Tag::AND);
Word *const Word::or_ = new Word("||", Tag::OR);
Word *const Word::eq_ = new Word("==", Tag::EQ);
Word *const Word::ne_ = new Word("!=", Tag::NE);
Word *const Word::le_ = new Word("<=", Tag::LE);
Word *const Word::ge_ = new Word(">=", Tag::GE);
Word *const Word::minus_ = new Word("minus", Tag::MINUS);
Word *const Word::true_ = new Word("true", Tag::TRUE);
Word *const Word::false_ = new Word("false", Tag::FALSE);
Word *const Word::temp_ = new Word("t", Tag::TEMP);