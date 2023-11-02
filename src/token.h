#ifndef POSTFIX_TOKEN_H
#define POSTFIX_TOKEN_H

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
    virtual precedence_t get_precedence() const = 0;

    virtual num_operands_t get_num_operands() = 0;

    virtual util::vector_t<precedence_t> get_valid_prev_token_prec() = 0;

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
    typename expr_push_strategy,
    typename get_valid_prev_token_strategy>
class owning_token_model_t: public token_concept_t {
public:

    explicit owning_token_model_t(
        tokenT in_token,
        calc_process_strategy in_calc_strat,
        expr_push_strategy in_expr_push_strat,
        get_valid_prev_token_strategy in_get_valid_strat
    ): 
        token( in_token ),
        calc_strat( in_calc_strat ),
        expr_push_strat( in_expr_push_strat ),
        get_valid_prev_token_strat( in_get_valid_strat )
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

    util::vector_t<precedence_t> get_valid_prev_token_prec() {
        return get_valid_prev_token_strat(token);
    }

    std::string get_name() {
        // is there need for separate strategy??
        return token.name;
    }

    precedence_t get_precedence() const {
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
    get_valid_prev_token_strategy get_valid_prev_token_strat;
};

} // namespace detail

class token_t {
public:

    token_t() {}

    template<
        typename tokenT,
        typename calc_process_strategy,
        typename expr_push_strategy,
        typename get_valid_prev_token_strategy>
    token_t(
        tokenT token,
        calc_process_strategy calc_strat,
        expr_push_strategy expr_strat,
        get_valid_prev_token_strategy valid_place_strat
    ): pimpl(
        util::make_unique<
            detail::owning_token_model_t<
                tokenT,
                calc_process_strategy,
                expr_push_strategy,
                get_valid_prev_token_strategy
            >
        >( token, calc_strat, expr_strat, valid_place_strat )
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

    util::vector_t<precedence_t> get_valid_prev_token_prec() {
        return pimpl->get_valid_prev_token_prec();
    }

    // get_name
    std::string get_name() {
        return pimpl->get_name();
    }

    // get_precedence
    precedence_t get_precedence() const{
        return pimpl->get_precedence();
    }

    bool is_valid_to_place_after(const token_t& before) {
        util::vector_t<precedence_t> valid_prev = get_valid_prev_token_prec();
        precedence_t prec = before.get_precedence();

        for(int i = 0; i < valid_prev.size(); ++i)
            if(valid_prev[i] == prec)
                return true;

        return false;
    }

private:
    util::unique_ptr<detail::token_concept_t> pimpl;
};


} // namespace postfix

#endif
