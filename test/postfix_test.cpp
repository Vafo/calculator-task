#include <catch2/catch_all.hpp>

#define POSTFIX_TEST 1

#include "postfix.h"

namespace postfix::detail {

// Methods of postfix_converter_impl_t are private, thus friend class is used
template<class... Ts>
class postfix_converter_impl_test {
public:
    postfix_converter_impl_test(): impl({
        util::shared_ptr<token_factory_base_t>(
            token_factory_t<Ts>()
        )...
    }) {}

    double test_number(std::string &in) {
        double res;
        impl.to_number(in.begin().base(), in.end().base(), res);
        return res;
    }


    // Direct wrappers

    const char *test_number(const char* begin, const char *end, double &res/*out*/) {
        return impl.to_number(begin, end, res);
    }

    const char *test_operator(const char* begin, const char *end, util::shared_ptr<token_base_t> &token/*out*/) {
        return impl.to_operator(begin, end, token);
    }

    const char *test_token(const char* begin, const char *end, util::shared_ptr<token_base_t> &token/*out*/) {
        return impl.to_token(begin, end, token);
    }

    postfix_converter_impl_t impl;
};

TEST_CASE("postfix_converter_impl_t: to_number", "[postfix_converter_impl_t][normal]") {
    postfix_converter_impl_test<
        token_minus_t,
        token_plus_t
    > test;
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
    postfix_converter_impl_test<
        token_minus_t,
        token_plus_t
    > test;

    const char in_str[] = "123 + 321 - 5";
    size_t str_size = sizeof(in_str) - 1; // exclude \0 symbol

    const char *iter = in_str;
    const char *prev_iter;
    util::shared_ptr<token_base_t> token;

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
    postfix_converter_impl_test<
        token_minus_t,
        token_plus_t
    > test;

    const char in_str[] = "123 + 321 - 5";
    size_t str_size = sizeof(in_str) - 1; // exclude \0 symbol

    const char *iter = in_str;
    util::shared_ptr<token_base_t> token;
    // all tokens are valid
    while(iter != (in_str + str_size)) {
        REQUIRE_NOTHROW(iter = test.test_token(iter, in_str + str_size, token));
    }

}

} // namespace postfix
