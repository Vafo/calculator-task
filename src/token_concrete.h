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
//     static const token_precedence prec;
// }

/* Operands */

class token_number {
public:
    double number;

    token_number(double in_num = 0): number(in_num) {}

    static const std::string name;
    static const token_precedence prec = token_precedence::number;
    static const num_operands_t num_operands = 0;
    static const util::vector<token_precedence> valid_prev_tokens;
};


/* Grammar */

class token_left_parenthesis {
public:
    static const std::string name;
    static const token_precedence prec = token_precedence::left_parenthesis;
    static const num_operands_t num_operands = 0;
    static const util::vector<token_precedence> valid_prev_tokens;
};

class token_right_parenthesis {
public:
    static const std::string name;
    static const token_precedence prec = token_precedence::right_paranthesis;
    static const num_operands_t num_operands = 0;
    static const util::vector<token_precedence> valid_prev_tokens;
};

class token_comma {
public:
    static const std::string name;
    static const token_precedence prec = token_precedence::comma;
    static const num_operands_t num_operands = 0;
    static const util::vector<token_precedence> valid_prev_tokens;
};


/* Operators */

class token_plus {
public:
    static const std::string name;
    static const token_precedence prec = token_precedence::add_n_sub;
    static const num_operands_t num_operands = 2;
    static const util::vector<token_precedence> valid_prev_tokens;
};

class token_plus_unary {
public:
    static const std::string name;
    static const token_precedence prec = token_precedence::unary;
    static const num_operands_t num_operands = 1;
    static const util::vector<token_precedence> valid_prev_tokens;
};

class token_minus {
public:
    static const std::string name;
    static const token_precedence prec = token_precedence::add_n_sub;
    static const num_operands_t num_operands = 2;
    static const util::vector<token_precedence> valid_prev_tokens;
};

class token_minus_unary {
public:
    static const std::string name;
    static const token_precedence prec = token_precedence::unary;
    static const num_operands_t num_operands = 1;
    static const util::vector<token_precedence> valid_prev_tokens;
};

class token_multiplication {
public:
    static const std::string name;
    static const token_precedence prec = token_precedence::multiplication;
    static const num_operands_t num_operands = 2;
    static const util::vector<token_precedence> valid_prev_tokens;
};

class token_division {
public:
    static const std::string name;
    static const token_precedence prec = token_precedence::multiplication;
    static const num_operands_t num_operands = 2;
    static const util::vector<token_precedence> valid_prev_tokens;
};

/* Functions */

class token_exp {
public:
    static const std::string name;
    static const token_precedence prec = token_precedence::function;
    static const num_operands_t num_operands = 2;
    static const util::vector<token_precedence> valid_prev_tokens;
};


/* Strategies */
namespace token_strategies {

/* expr_push_strategy template */
// Callable
// void func_name(
//     tokenT& tok,
//     util::vector<token> &expr,
//     util::stack<token> &st,
//     detail::token_concept *source_obj
// )

template<typename tokenT>
inline void do_push_all_until_left_paren(
    tokenT& tok,
    util::vector<token> &expr,
    util::stack<token> &st,
    detail::token_concept *source_obj
) {
    // retrieve precedence (properties) of token
    while(!st.empty())
        if(st.peek().get_name() != "(") {
            expr.push_back(st.peek());
            st.pop();
        } else {
            break;
        }
    
    // if stack is empty or top token is not left parenthesis
    if(st.empty() || st.peek().get_precedence() != token_precedence::left_parenthesis)
        throw std::logic_error("token_right_parenthesis::expr_push_logic(): left parenthesis is missing");
}

template<typename tokenT>
inline void do_push_all_including_left_paren(
    tokenT& tok,
    util::vector<token> &expr,
    util::stack<token> &st,
    detail::token_concept *source_obj
) {
    do_push_all_until_left_paren(tok, expr, st, source_obj);

    // remove left parenthesis
    st.pop();
}


/* calc_process_strategy template */
// Callable
// void func_name(
//     tokenT& tok,
//     util::stack<double> &st
// )

// Number strategy
inline void do_push_number_to_stack(
    token_number& tok,
    util::stack<double> &st
) {
    st.push(tok.number);
}


/* Functors */
/* Used by Strategies */

/* Set of all actual functions performed by operations */ 
class calc_process_token_funcs {
public:
    /* General interface */
    // args are presented in original order
    // e.g. 1 + 2 -> args = {1, 2}
    // double operator() (tokenT& tok, util::vector<double> &args) {


    // token_plus function
    double operator() (token_plus& tok, util::vector<double>& args) {
        return args[0] + args[1];
    }

    // token_plus_unary function
    double operator() (token_plus_unary& tok, util::vector<double>& args) {
        return args[0];
    }

    // token_minus function
    double operator() (token_minus& tok, util::vector<double>& args) {
        return args[0] - args[1];
    }

    // token_minus_unary function
    double operator() (token_minus_unary& tok, util::vector<double>& args) {
        return -args[0];
    }

    // token_multiplication function
    double operator() (token_multiplication& tok, util::vector<double>& args) {
        return args[0] * args[1];
    }

    // token_division function
    double operator() (token_division& tok, util::vector<double>& args) {
        return args[0] / args[1];
    }

    // token_exp function
    double operator() (token_exp& tok, util::vector<double>& args) {
        return std::pow(args[0], args[1]);
    }
};

class influence_ctx_token_grammar_funcs {
public:
    void operator() (token_left_parenthesis& tok, token_conversion_ctx& ctx) {
        ctx.parenthesis_commas.push(ctx.num_of_commas);
        ctx.num_of_commas = 0;
    }

    void operator() (token_comma& tok, token_conversion_ctx& ctx) {
        --ctx.parenthesis_commas.peek();
    }

    void operator() (token_right_parenthesis& tok, token_conversion_ctx& ctx) {
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
    void operator() (token_funcT& tok, token_conversion_ctx& ctx) {
        ctx.num_of_commas = tok.num_operands - 1;
    }
};


} // namespace token_strategies

} // namespace postfix

#endif