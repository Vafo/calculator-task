#ifndef TOKEN_BUILDER_H
#define TOKEN_BUILDER_H

#include "new_token.h"
#include "token_concrete.h"

namespace postfix::builder {

token_t number(double num);

token_t left_parenthesis();
token_t right_paranthesis();
token_t comma();

token_t plus();
token_t minus();


} // namespace postfix::builder

#endif