#ifndef TOKEN_CONCRETE_H
#define TOKEN_CONCRETE_H

/**
 * Token Specializations
 *      Token declarations
 *      Token Strategies
 *          Strategies
 *          Functors
*/

#include "token.h"

#include <cmath>

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
    static const util::vector<precedence_t> valid_prev_tokens;
};


/* Grammar */

class token_left_parenthesis {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::left_parenthesis;
    static const num_operands_t num_operands = 0;
    static const util::vector<precedence_t> valid_prev_tokens;
};

class token_right_parenthesis {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::right_paranthesis;
    static const num_operands_t num_operands = 0;
    static const util::vector<precedence_t> valid_prev_tokens;
};

class token_comma {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::comma;
    static const num_operands_t num_operands = 0;
    static const util::vector<precedence_t> valid_prev_tokens;
};


/* Operators */

class token_plus {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::add_n_sub;
    static const num_operands_t num_operands = 2;
    static const util::vector<precedence_t> valid_prev_tokens;
};

class token_plus_unary {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::unary;
    static const num_operands_t num_operands = 1;
    static const util::vector<precedence_t> valid_prev_tokens;
};

class token_minus {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::add_n_sub;
    static const num_operands_t num_operands = 2;
    static const util::vector<precedence_t> valid_prev_tokens;
};

class token_minus_unary {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::unary;
    static const num_operands_t num_operands = 1;
    static const util::vector<precedence_t> valid_prev_tokens;
};

class token_multiplication {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::multiplication;
    static const num_operands_t num_operands = 2;
    static const util::vector<precedence_t> valid_prev_tokens;
};

class token_division {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::multiplication;
    static const num_operands_t num_operands = 2;
    static const util::vector<precedence_t> valid_prev_tokens;
};

/* Functions */

class token_exp {
public:
    static const std::string name;
    static const precedence_t prec = precedence_t::function;
    static const num_operands_t num_operands = 2;
    static const util::vector<precedence_t> valid_prev_tokens;
};


/* Strategies */
namespace token_strategies {

/* expr_push_strategy template */
// Callable
// void func_name(
//     tokenT& token,
//     util::vector<token_t> &expr,
//     util::stack<token_t> &st,
//     detail::token_concept_t *source_obj
// )

template<typename tokenT>
inline void do_push_all_until_left_paren(
    tokenT& token,
    util::vector<token_t> &expr,
    util::stack<token_t> &st,
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
    util::vector<token_t> &expr,
    util::stack<token_t> &st,
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
//     util::stack<double> &st
// )

// Number strategy
inline void do_push_number_to_stack(
    token_number& token,
    util::stack<double> &st
) {
    st.push(token.number);
}


/* Functors */
/* Used by Strategies */

/* Set of all actual functions performed by operations */ 
class calc_process_token_funcs {
public:
    /* General interface */
    // args are presented in original order
    // e.g. 1 + 2 -> args = {1, 2}
    // double operator() (tokenT& token, util::vector<double> &args) {


    // token_plus function
    double operator() (token_plus& token, util::vector<double>& args) {
        return args[0] + args[1];
    }

    // token_plus_unary function
    double operator() (token_plus_unary& token, util::vector<double>& args) {
        return args[0];
    }

    // token_minus function
    double operator() (token_minus& token, util::vector<double>& args) {
        return args[0] - args[1];
    }

    // token_minus_unary function
    double operator() (token_minus_unary& token, util::vector<double>& args) {
        return -args[0];
    }

    // token_multiplication function
    double operator() (token_multiplication& token, util::vector<double>& args) {
        return args[0] * args[1];
    }

    // token_division function
    double operator() (token_division& token, util::vector<double>& args) {
        return args[0] / args[1];
    }

    // token_exp function
    double operator() (token_exp& token, util::vector<double>& args) {
        return std::pow(args[0], args[1]);
    }
};

class influence_ctx_token_grammar_funcs {
public:
    void operator() (token_left_parenthesis& token, token_conversion_ctx& ctx) {
        ctx.parenthesis_commas.push(ctx.num_of_commas);
        ctx.num_of_commas = 0;
    }

    void operator() (token_comma& token, token_conversion_ctx& ctx) {
        --ctx.parenthesis_commas.peek();
    }

    void operator() (token_right_parenthesis& token, token_conversion_ctx& ctx) {
        if(ctx.parenthesis_commas.size() == 0)
            throw std::logic_error("there is no corresponding left parenthesis");

        if(ctx.parenthesis_commas.peek() > 0)
            throw std::logic_error("too much commas");
        else if(ctx.parenthesis_commas.peek() < 0)
            throw std::logic_error("not enough commas");

        ctx.parenthesis_commas.pop();
    }
};

class influence_ctx_token_func_funcs {
public:
    template<typename token_funcT>
    void operator() (token_funcT& token, token_conversion_ctx& ctx) {
        ctx.num_of_commas = token.num_operands - 1;
    }
};


} // namespace token_strategies

} // namespace postfix

#endif