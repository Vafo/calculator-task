#ifndef TOKEN_CONCRETE_H
#define TOKEN_CONCRETE_H

#include "token.h"

namespace postfix {

// Tokens

/* Token Template */
// class token_concrete {
// public:
//     static const std::string name;
//     static const precedence_t prec;
// }

/* Operands */

class token_number {
public:
    double number;

    token_number(double in_num = 0): number(in_num) {}

    static const std::string name;
    static const precedence_t prec = precedence_t::number;
    static const num_operands_t num_operands = 0;
};


/* Grammar */

class token_left_parenthesis {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::left_parenthesis;
    static const num_operands_t num_operands = 0;
};

class token_right_parenthesis {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::right_paranthesis;
    static const num_operands_t num_operands = 0;
};

class token_comma {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::comma;
    static const num_operands_t num_operands = 0;
};


/* Operators */

class token_plus {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::add_n_sub;
    static const num_operands_t num_operands = 2;
};

class token_minus {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::add_n_sub;
    static const num_operands_t num_operands = 2;
};


/* Set of all actual functions performed by operations */ 
class token_apply_functions {
public:
    /* General interface */
    // args are presented in original order
    // e.g. 1 + 2 -> args = {1, 2}
    // double operator() (tokenT& token, util::vector_t<double> &args) {


    // token_plus function
    double operator() (token_plus& token, util::vector_t<double>& args) {
        return args[0] + args[1];
    }

    // token_minus function
    double operator() (token_minus& token, util::vector_t<double>& args) {
        return args[0] - args[1];
    }
};

/* Strategies */
namespace token_strategies {

/* expr_push_strategy template */
// Callable
// void func_name(
//     tokenT& token,
//     util::vector_t<token_t> &expr,
//     util::stack_t<token_t> &st,
//     detail::token_concept_t *source_obj
// )

template<typename tokenT>
inline void do_push_all_until_left_paren(
    tokenT& token,
    util::vector_t<token_t> &expr,
    util::stack_t<token_t> &st,
    detail::token_concept_t *source_obj
) {
    // retrieve precedence (properties) of token_t
    while(!st.empty())
        if(st.peek().get_name() != "(") {
            expr.push_back(st.peek());
            st.pop();
        } else {
            break;
        }
    
    // if stack is empty or top token is not left parenthesis
    if(st.empty() || st.peek().get_precedence() != precedence_t::left_parenthesis)
        throw std::logic_error("token_right_parenthesis::expr_push_logic(): left parenthesis is missing");
}

template<typename tokenT>
inline void do_push_all_including_left_paren(
    tokenT& token,
    util::vector_t<token_t> &expr,
    util::stack_t<token_t> &st,
    detail::token_concept_t *source_obj
) {
    do_push_all_until_left_paren(token, expr, st, source_obj);

    // remove left parenthesis
    st.pop();
}


/* calc_process_strategy template */
// Callable
// void func_name(
//     tokenT& token,
//     util::stack_t<double> &st
// )

// Number strategy
inline void do_push_number_to_stack(
    token_number& token,
    util::stack_t<double> &st
) {
    st.push(token.number);
}

} // namespace token_strategies

} // namespace postfix

#endif