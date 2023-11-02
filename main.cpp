#include <iostream>
#include <iterator>

#include <string>

#include "postfix.h"


int main(int argc, char *argv[]) {

    postfix::postfix_converter_t converter;
    postfix::postfix_expr_t expr;
    std::string in_str;

    std::cout << "Enter expressions" << std::endl;
    while(std::getline(std::cin, in_str)) {
        expr = converter.convert(in_str);
        std::cout << expr.evaluate() << std::endl;
    }

    return 0;
}