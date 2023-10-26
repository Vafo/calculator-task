#include <iostream>
#include <iterator>

#include <string>

#include "postfix.h"
#include "token.h"

#include "util/shared_ptr.h"
#include "util/container.h"


int main(int argc, char *argv[]) {

    postfix::postfix_converter_t<
        postfix::token_plus_t,
        postfix::token_minus_t
    > converter;

    postfix::postfix_expr_t expr = converter.convert("1 + 4 - 5");

    std::cout << expr.evaluate();

    return 0;
}