#include "token_builder.h"

namespace postfix::builder {

token_t number(double num) {
    token_number tok_num(num);
    token_t token(
        tok_num,
        token_strategies::do_push_number_to_stack,
        token_strategies::do_push_itself_to_expr<token_number>,
        token_strategies::do_get_valid_prev_token<token_number>
    );

    return token;
}

token_t left_parenthesis() {
    using left_par_t = token_left_parenthesis;
    left_par_t left_par;
    token_t token(
        left_par,
        token_strategies::do_calc_throw<left_par_t>,
        token_strategies::do_push_itself_to_stack<left_par_t>,
        token_strategies::do_get_valid_prev_token<left_par_t>
    );

    return token;
}

token_t right_paranthesis() {
    using right_par_t = token_right_parenthesis;
    right_par_t right_par;
    token_t token(
        right_par,
        token_strategies::do_calc_throw<right_par_t>,
        token_strategies::do_push_all_including_left_paren<right_par_t>,
        token_strategies::do_get_valid_prev_token<right_par_t>
    );

    return token;
}

token_t comma() {
    using comma_t = token_comma;
    comma_t com;
    token_t token(
        com,
        token_strategies::do_calc_throw<comma_t>,
        token_strategies::do_push_all_until_left_paren<comma_t>,
        token_strategies::do_get_valid_prev_token<comma_t>
    );

    return token;
}

token_t plus() {
    token_plus plus;
    token_t token(
        plus,
        token_strategies::do_calc_apply<token_plus, token_apply_functions>,
        token_strategies::do_push_with_precedence<token_plus>,
        token_strategies::do_get_valid_prev_token<token_plus>
    );

    return token;
}

token_t plus_unary() {
    using plus_un_t = token_plus_unary;
    plus_un_t plus_un;
    token_t token(
        plus_un,
        token_strategies::do_calc_apply<plus_un_t, token_apply_functions>,
        token_strategies::do_push_with_precedence<plus_un_t>,
        token_strategies::do_get_valid_prev_token<plus_un_t>
    );

    return token;
}

token_t minus() {
    using minus_t = token_minus;
    minus_t minus;
    token_t token(
        minus,
        token_strategies::do_calc_apply<minus_t, token_apply_functions>,
        token_strategies::do_push_with_precedence<minus_t>,
        token_strategies::do_get_valid_prev_token<minus_t>
    );

    return token;
}

token_t minus_unary() {
    using minus_un_t = token_minus_unary;
    minus_un_t minus_un;
    token_t token(
        minus_un,
        token_strategies::do_calc_apply<minus_un_t, token_apply_functions>,
        token_strategies::do_push_with_precedence<minus_un_t>,
        token_strategies::do_get_valid_prev_token<minus_un_t>
    );

    return token;
}

token_t multiplication() {
    using multi_t = token_multiplication;
    multi_t multi;
    token_t token(
        multi,
        token_strategies::do_calc_apply<multi_t, token_apply_functions>,
        token_strategies::do_push_with_precedence<multi_t>,
        token_strategies::do_get_valid_prev_token<multi_t>
    );

    return token;
}

token_t division() {
    using div_t = token_division;
    div_t division;
    token_t token(
        division,
        token_strategies::do_calc_apply<div_t, token_apply_functions>,
        token_strategies::do_push_with_precedence<div_t>,
        token_strategies::do_get_valid_prev_token<div_t>
    );

    return token;
}

token_t exp() {
    using exp_t = token_exp;
    exp_t exp;
    token_t token(
        exp,
        token_strategies::do_calc_apply<exp_t, token_apply_functions>,
        token_strategies::do_push_with_precedence<exp_t>,
        token_strategies::do_get_valid_prev_token<exp_t>
    );

    return token;
}
} // namespace postfix::builder