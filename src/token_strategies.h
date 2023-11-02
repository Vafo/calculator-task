#ifndef TOKEN_STRATEGIES_H
#define TOKEN_STRATEGIES_H

#include "token.h"

// Strategies
namespace postfix::token_strategies {

/* expr_push Strategies */

template<typename tokenT>
inline void do_push_itself_to_stack(
    tokenT& token,
    util::vector_t<token_t> &expr,
    util::stack_t<token_t> &st,
    detail::token_concept_t *source_obj
) {
    token_t token_obj(source_obj->clone());
    st.push(token_obj);
}

template<typename tokenT>
inline void do_push_itself_to_expr(
    tokenT& token,
    util::vector_t<token_t> &expr,
    util::stack_t<token_t> &st,
    detail::token_concept_t *source_obj
) {
    token_t token_obj(source_obj->clone());
    expr.push_back(token_obj);
}

template<typename tokenT>
inline void do_push_with_precedence(
    tokenT& token,
    util::vector_t<token_t> &expr,
    util::stack_t<token_t> &st,
    detail::token_concept_t *source_obj
) {
    precedence_t prec = token.prec;
    while(!st.empty())
        if(prec <= st.peek().get_precedence()) {
            expr.push_back(st.peek());
            st.pop();
        } else {
            break;
        }

    do_push_itself_to_stack(token, expr, st, source_obj);
}


/* calc_process Strategies */

template<typename tokenT>
inline void do_calc_throw(
    tokenT& token,
    util::stack_t<double> &st
) {
    std::string err_msg = 
        "do_calc_throw: the token " +
        token.name +
        " is not supposed to be in postfix expression to be calculated";
    
    throw std::logic_error(err_msg);
}

template<typename tokenT, typename OperatorFunction>
inline void do_calc_apply(
    tokenT& token,
    util::stack_t<double> &st
) {
    if(st.size() < token.num_operands) {
        std::string err_msg = 
            "do_calc_apply: operator " +
            token.name +
            " does not have enough operands";

        throw std::domain_error(err_msg);
    }

    // Collect all arguments, in original order
    util::vector_t<double> func_args(token.num_operands);
    for(int i = token.num_operands - 1; i >= 0; --i) {
        func_args[i] = st.peek();
        st.pop();
    }
    
    OperatorFunction functor;
    st.push(functor(token, func_args));
}

/* get_valid_prev_token Strategies */

template<typename tokenT>
util::vector_t<precedence_t> do_get_valid_prev_token(tokenT& token) {
    return token.valid_prev_tokens;
}

} // namespace postfix::token_strategies


#endif