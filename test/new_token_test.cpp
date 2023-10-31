#include "catch2/catch_all.hpp"

#include "new_token.h"
#include "token_concrete.h"
#include "token_builder.h"

namespace postfix {

TEST_CASE("new_token: creation of object", "[new_token]") {
    token_t token = builder::number(50);
}


TEST_CASE("new_token: direct calculation", "[new_token]") {
    const int 
        val1 = -5,
        val2 = 312;

    token_t num1 = builder::number(val1);
    token_t num2 = builder::number(val2);
    token_t plus = builder::plus();
    
    // stack required for postfix calculation
    util::stack_t<double> st;

    SECTION("num1 + num2") {
        util::vector_t<token_t> tokens = {num1, num2, plus};

        for(int i = 0; i < tokens.size(); ++i)
            tokens[i].calc_process(st);

        REQUIRE(st.peek() == (val1 + val2));
    }

    SECTION("num1 + num2 + num2") {
        // postifx notation: num1 num2 + num2 +
        util::vector_t<token_t> tokens = {num1, num2, plus, num2, plus};

        for(int i = 0; i < tokens.size(); ++i)
            tokens[i].calc_process(st);

        REQUIRE(st.peek() == (val1 + val2 + val2));
    }

}

TEST_CASE("new_token: minus sign test", "[new_token]") {
    const int
        val1 = 412,
        val2 = -432;

    token_t num1 = builder::number(val1);
    token_t num2 = builder::number(val2);

    token_t plus = builder::plus();
    token_t minus = builder::minus();

    // stack required for postfix calculation
    util::stack_t<double> st;

    SECTION("num1 - num2") {
        util::vector_t<token_t> tokens = {num1, num2, minus};
        
        for(int i = 0; i < tokens.size(); ++i)
            tokens[i].calc_process(st);

        REQUIRE(st.peek() == (val1 - val2));
    }

    SECTION("num1 - num2 + num2") {
        // postfix expr: num1 num2 minus num2 plus
        util::vector_t<token_t> tokens = {num1, num2, minus, num2, plus};
        
        for(int i = 0; i < tokens.size(); ++i)
            tokens[i].calc_process(st);

        REQUIRE(st.peek() == (val1 - val2 + val2));
    }
}

} // namespace postfix