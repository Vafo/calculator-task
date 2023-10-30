#ifndef POSTFIX_TOKEN_H
#define POSTFIX_TOKEN_H

#include <stdexcept>

#include "util/vector.h"
#include "util/stack.h"

#include "util/unique_ptr.h"

namespace postfix {

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

    virtual 
    util::unique_ptr<token_concept_t> clone() = 0;

    // Virtual destructor, to avoid unique_ptr mem leak
    virtual ~token_concept_t() {}
};

template<
    typename tokenT,
    typename calc_process_strategy,
    typename expr_push_strategy>
class owning_token_model_t: public token_concept_t {
public:

    owning_token_model_t(
        tokenT in_token,
        calc_process_strategy in_calc_strat,
        expr_push_strategy in_expr_push_strat
    ): token(in_token), calc_strat(in_calc_strat), expr_push_strat(in_expr_push_strat) {}

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

    util::unique_ptr<token_concept_t> clone() {
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

    template<
        typename tokenT,
        typename calc_process_strategy,
        typename expr_push_strategy>
    token_t(
        tokenT& token,
        calc_process_strategy& calc_strat,
        expr_push_strategy& expr_strat
    ): pimpl(
        util::make_unique<
            detail::owning_token_model_t<tokenT, calc_process_strategy, expr_push_strategy>
        >(token, calc_strat, expr_strat)
    ) {}

    void expr_push(
        util::vector_t<token_t> &expr,
        util::stack_t<token_t> &st
    ) {
        pimpl->expr_push(expr, st);
    }

    void calc_process(util::stack_t<double> &st) {
        pimpl->calc_process(st);
    }

private:
    util::unique_ptr<detail::token_concept_t> pimpl;
};

// Strategies



} // namespace postfix

#endif
