#include <catch2/catch_all.hpp>

#define POSTFIX_TEST 1

#include "postfix.h"

namespace postfix::detail {

// Methods of postfix_converter_impl_t are private, thus friend class is used

class postfix_converter_impl_test {
public:
    postfix_converter_impl_test(): 
    impl({ /* initializer-list */
        builder::left_parenthesis(),
        builder::right_paranthesis(),
        builder::comma(),
        
        builder::plus(),
        builder::minus(),   
    }) { }

    double test_number(std::string &in) {
        double res;
        impl.to_number(in.begin().base(), in.end().base(), res);
        return res;
    }


    // Direct wrappers

    const char *test_number(const char* begin, const char *end, double &res/*out*/) {
        return impl.to_number(begin, end, res);
    }

    const char *test_operator(
        const char* begin, const char *end,
        token_t& token/*out*/
    ) {
        util::vector_t<token_t> candidates;
        const char *iter = impl.to_operator(begin, end, candidates);
        
        token = candidates[0];
        return iter;
    }

    const char *test_token(
        const char* begin, const char *end,
        token_t& token/*out*/
    ) {
        util::vector_t<token_t> candidates;
        const char *iter = impl.to_token(begin, end, candidates);
        token = candidates[0];
        return iter;
    }

    postfix_converter_impl_t impl;
};

TEST_CASE("postfix_converter_impl_t: to_number", "[postfix_converter_impl_t][normal]") {
    postfix_converter_impl_test test;
    std::string val = "123.12";
    REQUIRE(123.12 == test.test_number(val));
    val = "234.0001";
    REQUIRE(234.0001 == test.test_number(val));

    val = "0.0001";
    REQUIRE(0.0001 == test.test_number(val));

    val = "0.0";
    REQUIRE(0.0 == test.test_number(val));

    val = "1";
    REQUIRE(1 == test.test_number(val));

    const char in_cstr[] = "123 + 321";
    size_t str_size = sizeof(in_cstr) - 1; // exclude \0 symbol
    double num;
    const char *iter = test.test_number(in_cstr, in_cstr + str_size, num);
    REQUIRE(iter == in_cstr + 3);
    REQUIRE(num == 123);

    // iter did not move, as it did not encounter number starting at in_cstr[3]
    iter = test.test_number(iter, in_cstr + str_size, num);
    REQUIRE(iter == in_cstr + 3);

    // move iter to + sign
    iter++;
    // iter did not move, as it did not encounter number starting at in_cstr[4]
    iter = test.test_number(iter, in_cstr + str_size, num);
    REQUIRE(iter == in_cstr + 4);

    // move iter past + sign
    iter++;
    // Retrieve last number
    iter = test.test_number(iter, in_cstr + str_size, num);
    REQUIRE(iter == in_cstr + str_size);
    REQUIRE(num == 321);
}

TEST_CASE("postfix_converter_impl_t: to_operator", "[postfix_converter_impl_t][normal]") {
    postfix_converter_impl_test test;

    const char in_str[] = "123 + 321 - 5";
    size_t str_size = sizeof(in_str) - 1; // exclude \0 symbol

    const char *iter = in_str;
    const char *prev_iter;
    token_t token;

    // iter points to invalid operator (number 123)
    REQUIRE_THROWS(test.test_operator(iter, in_str + str_size, token));

    // move iter to valid operator
    iter += 3;
    prev_iter = iter;
    REQUIRE_NOTHROW(iter = test.test_operator(iter, in_str + str_size, token));
    REQUIRE(iter != prev_iter);
    
    // iter points to invalid operator
    REQUIRE_THROWS(iter = test.test_operator(iter, in_str + str_size, token));
    
    // move iter to next valid operator (- sign)
    iter += 4;
    prev_iter = iter;
    REQUIRE_NOTHROW(iter = test.test_operator(iter, in_str + str_size, token));
    REQUIRE(iter != prev_iter);

    // iter points to invalid operator
    prev_iter = iter;
    REQUIRE_THROWS(iter = test.test_operator(iter, in_str + str_size, token));
    REQUIRE(iter == prev_iter);
}

TEST_CASE("postfix_converter_impl_t: to_token", "[postfix_converter_impl_t][normal]") {
    postfix_converter_impl_test test;

    const char in_str[] = "123 + 321 - 5";
    size_t str_size = sizeof(in_str) - 1; // exclude \0 symbol

    const char *iter = in_str;
    token_t token;
    // all tokens are valid
    while(iter != (in_str + str_size)) {
        REQUIRE_NOTHROW(iter = test.test_token(iter, in_str + str_size, token));
    }
}

TEST_CASE("postfix_converter_t: conversion", "[postfix_converter_t][normal]") {
    postfix_converter_t converter;

    // Some working examples

    postfix_expr_t expr = converter.convert("1 + 2");
    REQUIRE(expr.evaluate() == 3);

    expr = converter.convert("3-5");
    REQUIRE(expr.evaluate() == -2);

    postfix_expr_t exp2 = converter.convert("10 + (5 - 10) - (3 - 5)");
    REQUIRE(exp2.evaluate() == (10 + (5 - 10) - (3 - 5)) );
}

TEST_CASE("postfix_converter_t: multiplication/division", "[postfix_converter_t]") {
    postfix_converter_t converter;
    postfix_expr_t expr;

    expr = converter.convert("5 * 3 - 1");
    REQUIRE(expr.evaluate() == (5 * 3 - 1));

    expr = converter.convert("(5 * 3 / 2) * (3 + 0 - 5) ");
    REQUIRE(expr.evaluate() == ( (5 * ( (double) 3) / 2) * (3 + 0 - 5) ));
}

TEST_CASE("postfix_converter_t: unary plus & minus", "[postfix_converter_t]") {
    postfix_converter_t converter;
    postfix_expr_t expr;

    expr = converter.convert("-5 + 3 - 5");
    REQUIRE(expr.evaluate() == ( -5 + 3 - 5 ));

    REQUIRE_THROWS( expr = converter.convert("-5 * -2") );
    REQUIRE_THROWS( expr = converter.convert("-5 * +2") );

    expr = converter.convert("-5 * 3 - 5");
    REQUIRE(expr.evaluate() == ( -5 * 3 - 5 ));

    expr = converter.convert("-5 * (-3 - 5)");
    REQUIRE(expr.evaluate() == ( -5 * (-3 - 5) ));

    expr = converter.convert("-123");
    REQUIRE(expr.evaluate() == ( -123 ));

    expr = converter.convert("-(+123)");
    REQUIRE(expr.evaluate() == ( -(+123) ));

    expr = converter.convert("-(-123)");
    REQUIRE(expr.evaluate() == ( -(-123) ));

    expr = converter.convert("-(-123 + 21)");
    REQUIRE(expr.evaluate() == ( -(-123 + 21) ));
}

TEST_CASE("postfix_converter_t: exp function", "[postfix_converter_t]") {
    postfix_converter_t converter;
    postfix_expr_t expr;

    expr = converter.convert("exp(2,3)");
    REQUIRE(expr.evaluate() == (8));

    expr = converter.convert("exp(2,0)");
    REQUIRE(expr.evaluate() == (1));

    expr = converter.convert("exp(1,3)");
    REQUIRE(expr.evaluate() == (1));

    expr = converter.convert("exp(5,3)");
    REQUIRE(expr.evaluate() == (125));

    expr = converter.convert("-exp(2,3)");
    REQUIRE(expr.evaluate() == (-8));

    expr = converter.convert("+exp(2,-1)");
    REQUIRE(expr.evaluate() == (0.5));
}

} // namespace postfix
