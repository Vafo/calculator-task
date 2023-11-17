#ifndef TOKEN_BUILDER_H
#define TOKEN_BUILDER_H

#include "token.h"
#include "token_concrete.h"

namespace postfix::builder {

token number(double num);

token left_parenthesis();
token right_paranthesis();
token comma();

token plus();
token plus_unary();
token minus();
token minus_unary();
token multiplication();
token division();

token exp();

} // namespace postfix::builder

#endif