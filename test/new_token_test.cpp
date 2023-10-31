#include "catch2/catch_all.hpp"

#include "new_token.h"
#include "token_concrete.h"

namespace postfix {

token_t build_token_number(double num) {
    token_number tok_num(num);
    token_t token(
        tok_num,
        token_strategies::do_push_number_to_stack,
        token_strategies::do_push_itself_to_expr<token_number>
    );

    return token;
}

token_t build_token_plus() {
    token_plus plus;
    token_t token(
        plus,
        token_strategies::do_calc_apply<token_plus, token_apply_functions>,
        token_strategies::do_push_with_precedence<token_plus>
    );

    return token;
}

TEST_CASE("new_token: creation of object", "[new_token]") {
    token_t token = build_token_number(50);
}


TEST_CASE("new_token: direct calculation", "[new_token]") {
    const int 
        val1 = -5,
        val2 = 312;

    token_t num1 = build_token_number(val1);
    token_t num2 = build_token_number(val2);
    token_t plus = build_token_plus();

    SECTION("num1 + num2") {
        util::vector_t<token_t> tokens = {num1, num2, plus};
        util::stack_t<double> st;

        for(int i = 0; i < tokens.size(); ++i)
            tokens[i].calc_process(st);

        REQUIRE(st.peek() == (val1 + val2));
    }

    SECTION("num1 + num2 + num2") {
        // postifx notation: num1 num2 + num2 +
        util::vector_t<token_t> tokens = {num1, num2, plus, num2, plus};
        util::stack_t<double> st;

        for(int i = 0; i < tokens.size(); ++i)
            tokens[i].calc_process(st);

        REQUIRE(st.peek() == (val1 + val2 + val2));
    }

}

} // namespace postfix