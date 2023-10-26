#include "token.h"

namespace postfix {

namespace detail {

void do_push_upto_left_paren(
    util::vector_t<util::shared_ptr<token_base_t>> &expr,
    util::stack_t<util::shared_ptr<token_base_t>> &st
) {
    while(!st.empty())
        if(st.peek()->prec != precedence_t::left_parenthesis) {
            expr.push_back(st.peek());
            st.pop();
        } else {
            break;
        }
    
    // if stack is empty or top token is not left parenthesis
    if(st.empty() || st.peek()->prec != precedence_t::left_parenthesis)
        throw std::logic_error("token_right_parenthesis::expr_push_logic(): left parenthesis is missing");
}

void do_push_with_precedence(
    precedence_t prec,
    util::vector_t<util::shared_ptr<token_base_t>> &expr,
    util::stack_t<util::shared_ptr<token_base_t>> &st
) {
    while(!st.empty())
        if(prec <= st.peek()->prec) {
            expr.push_back(st.peek());
            st.pop();
        } else {
            break;
        }
}

} // namespace detail

const char * const token_base_t::token_name = NULL;
const char * const token_number_t::token_name = NULL;
const char * const token_plus_t::token_name = "+";
const char * const token_minus_t::token_name = "-";
const char * const token_comma_t::token_name = ",";
const char * const token_left_parenthesis_t::token_name = "(";
const char * const token_right_parenthesis_t::token_name = ")";

} // namespace postfix
