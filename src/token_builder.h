#ifndef TOKEN_BUILDER_H
#define TOKEN_BUILDER_H

#include "token.h"
#include "token_concrete.h"

namespace postfix::builder {

token_t number(double num);

token_t left_parenthesis();
token_t right_paranthesis();
token_t comma();

token_t plus();
token_t plus_unary();
token_t minus();
token_t minus_unary();
token_t multiplication();
token_t division();

token_t exp();

} // namespace postfix::builder

#endif