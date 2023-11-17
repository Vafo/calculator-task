#ifndef TOKEN_STRATEGIES_H
#define TOKEN_STRATEGIES_H

/**
 * General Strategies, not tied to specific token
*/

#include "token_general.h"

// Strategies
namespace postfix::token_strategies {

/* expr_push Strategies */

template<typename tokenT>
inline void do_push_itself_to_stack(
    tokenT& tok,
    util::vector<token> &expr,
    util::stack<token> &st,
    detail::token_concept *source_obj
) {
    token token_obj(source_obj->clone());
    st.push(token_obj);
}

template<typename tokenT>
inline void do_push_itself_to_expr(
    tokenT& tok,
    util::vector<token> &expr,
    util::stack<token> &st,
    detail::token_concept *source_obj
) {
    token token_obj(source_obj->clone());
    expr.push_back(token_obj);
}

template<typename tokenT>
inline void do_push_with_precedence(
    tokenT& tok,
    util::vector<token> &expr,
    util::stack<token> &st,
    detail::token_concept *source_obj
) {
    token_precedence prec = tok.prec;
    while(!st.empty())
        if(prec <= st.peek().get_precedence()) {
            expr.push_back(st.peek());
            st.pop();
        } else {
            break;
        }

    do_push_itself_to_stack(tok, expr, st, source_obj);
}


/* calc_process Strategies */

template<typename tokenT>
inline void do_calc_throw(
    tokenT& tok,
    util::stack<double> &st
) {
    std::string err_msg = 
        "do_calc_throw: the token " +
        tok.name +
        " is not supposed to be in postfix expression to be calculated";
    
    throw std::logic_error(err_msg);
}

template<typename tokenT, typename OperatorFunction>
inline void do_calc_apply(
    tokenT& tok,
    util::stack<double> &st
) {
    if(st.size() < tok.num_operands) {
        std::string err_msg = 
            "do_calc_apply: operator " +
            tok.name +
            " does not have enough operands";

        throw std::domain_error(err_msg);
    }

    // Collect all arguments, in original order
    util::vector<double> func_args(tok.num_operands);
    for(int i = tok.num_operands - 1; i >= 0; --i) {
        func_args[i] = st.peek();
        st.pop();
    }
    
    OperatorFunction functor;
    st.push(functor(tok, func_args));
}

/* get_valid_prev_token Strategies */

template<typename tokenT>
util::vector<token_precedence> do_get_valid_prev_token(tokenT& tok) {
    return tok.valid_prev_tokens;
}

/* influence_context Strategies */

template<typename tokenT>
inline void do_influence_ctx_nothing(tokenT& tok, token_conversion_ctx& ctx) {
    return; /*do nothing*/
}

template<typename tokenT, typename InfluenceFunction>
inline void do_influence_ctx_apply(tokenT& tok, token_conversion_ctx& ctx) {
    InfluenceFunction functor;
    functor(tok, ctx);
}

} // namespace postfix::token_strategies


#endif