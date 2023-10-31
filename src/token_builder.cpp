#include "token_builder.h"

namespace postfix::builder {

token_t number(double num) {
    token_number tok_num(num);
    token_t token(
        tok_num,
        token_strategies::do_push_number_to_stack,
        token_strategies::do_push_itself_to_expr<token_number>
    );

    return token;
}

token_t left_parenthesis() {
    using left_par_t = token_left_parenthesis;
    left_par_t left_par;
    token_t token(
        left_par,
        token_strategies::do_calc_throw<left_par_t>,
        token_strategies::do_push_itself_to_stack<left_par_t>
    );

    return token;
}

token_t right_paranthesis() {
    using right_par_t = token_right_parenthesis;
    right_par_t right_par;
    token_t token(
        right_par,
        token_strategies::do_calc_throw<right_par_t>,
        token_strategies::do_push_all_including_left_paren<right_par_t>
    );

    return token;
}

token_t comma() {
    using comma_t = token_comma;
    comma_t com;
    token_t token(
        com,
        token_strategies::do_calc_throw<comma_t>,
        token_strategies::do_push_all_until_left_paren<comma_t>
    );

    return token;
}

token_t plus() {
    token_plus plus;
    token_t token(
        plus,
        token_strategies::do_calc_apply<token_plus, token_apply_functions>,
        token_strategies::do_push_with_precedence<token_plus>
    );

    return token;
}

token_t minus() {
    token_minus minus;
    token_t token(
        minus,
        token_strategies::do_calc_apply<token_minus, token_apply_functions>,
        token_strategies::do_push_with_precedence<token_minus>
    );

    return token;
}

token_t multiplication() {
    token_multiplication multi;
    token_t token(
        multi,
        token_strategies::do_calc_apply<token_multiplication, token_apply_functions>,
        token_strategies::do_push_with_precedence<token_multiplication>
    );
    
    return token;
}

} // namespace postfix::builder