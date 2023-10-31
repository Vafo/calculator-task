#include <iostream>
#include <iterator>

#include <string>

#include "postfix.h"


int main(int argc, char *argv[]) {

    postfix::postfix_converter_t converter;

    postfix::postfix_expr_t expr = converter.convert("1 + 4 - 5 + 10 - (3 - 5)");

    std::cout << expr.evaluate();

    return 0;
}