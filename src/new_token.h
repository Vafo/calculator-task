#ifndef NEW_POSTFIX_TOKEN_H
#define NEW_POSTFIX_TOKEN_H

#include <stdexcept>

#include "util/vector.h"
#include "util/stack.h"

#include "util/unique_ptr.h"

namespace postfix {

typedef enum{
    number,
    comma,
    left_parenthesis,
    right_paranthesis,
    add_n_sub,
    multiplication,
    unary,
    function
} precedence_t;

typedef int num_operands_t;

// forward declaration
class token_t;

namespace detail {

class token_concept_t {
public:
    // calc_process
    virtual void calc_process(util::stack_t<double> &st) = 0;

    // expr_push 
    virtual void expr_push(
        util::vector_t<token_t> &expr,
        util::stack_t<token_t> &st
    ) = 0;

    // get name of token
    virtual std::string get_name() = 0;

    // get precedence of token
    virtual precedence_t get_precedence() = 0;

    virtual num_operands_t get_num_operands() = 0;

    virtual 
    util::unique_ptr<token_concept_t> clone() const = 0;

    // Virtual destructor, to avoid unique_ptr mem leak
    virtual ~token_concept_t() {}
};

/* Token Template */
// class token_concrete {
// public:
//     static const std::string name;
//     static const precedence_t prec;
// }

template<
    typename tokenT,
    typename calc_process_strategy,
    typename expr_push_strategy>
class owning_token_model_t: public token_concept_t {
public:

    explicit owning_token_model_t(
        tokenT in_token,
        calc_process_strategy in_calc_strat,
        expr_push_strategy in_expr_push_strat
    ): 
        token( in_token ),
        calc_strat( in_calc_strat ),
        expr_push_strat( in_expr_push_strat )
    {}

    void calc_process(util::stack_t<double> &st) { 
        calc_strat(token, st);
    }

    void expr_push(
        util::vector_t<token_t> &expr,
        util::stack_t<token_t> &st
    ) {
        // expr_push_strategy requires info to build token object
        expr_push_strat(token, expr, st, this);
    }

    std::string get_name() {
        // is there need for separate strategy??
        return token.name;
    }

    precedence_t get_precedence() {
        // is there need for separate strategy??
        return token.prec;
    }

    num_operands_t get_num_operands() {
        // num of operands of token (which maybe an operator)
        return token.num_operands;
    }

    util::unique_ptr<token_concept_t> clone() const{
        return util::make_unique<owning_token_model_t>( *this );
    }    

private:
    tokenT token;
    calc_process_strategy calc_strat;
    expr_push_strategy expr_push_strat;
};

} // namespace detail

class token_t {
public:

    token_t() {}

    template<
        typename tokenT,
        typename calc_process_strategy,
        typename expr_push_strategy>
    token_t(
        tokenT token,
        calc_process_strategy calc_strat,
        expr_push_strategy expr_strat
    ): pimpl(
        util::make_unique<
            detail::owning_token_model_t<tokenT, calc_process_strategy, expr_push_strategy>
        >( token, calc_strat, expr_strat)
    ) {}

    token_t(
        util::unique_ptr<detail::token_concept_t> pimpl_in
    ):
        pimpl( std::move(pimpl_in) )
    {}

    token_t(
        const token_t& other
    ):
        pimpl( other.pimpl->clone() )
    {}

    token_t&
    operator=(
        const token_t& other
    ) {
        pimpl = std::move( other.pimpl->clone() );
        return *this;
    }

    void expr_push(
        util::vector_t<token_t> &expr,
        util::stack_t<token_t> &st
    ) {
        pimpl->expr_push(expr, st);
    }

    void calc_process(util::stack_t<double> &st) {
        pimpl->calc_process(st);
    }

    // get_name
    std::string get_name() {
        return pimpl->get_name();
    }

    // get_precedence
    precedence_t get_precedence() {
        return pimpl->get_precedence();
    }

private:
    util::unique_ptr<detail::token_concept_t> pimpl;
};

// Strategies
namespace token_strategies {

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

} // namespace token_strategies


} // namespace postfix

#endif
