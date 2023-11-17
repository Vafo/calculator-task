#ifndef TOKEN_GENERAL_H
#define TOKEN_GENERAL_H

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
} token_precedence;

typedef int num_operands_t;

// forward declaration
class token;
class token_conversion_ctx;

namespace detail {

class token_concept {
public:
    // calc_process
    virtual void calc_process(util::stack<double> &st) = 0;

    // expr_push 
    virtual void expr_push(
        util::vector<token> &expr,
        util::stack<token> &st
    ) = 0;

    virtual void influence_ctx(token_conversion_ctx& ctx) = 0;

    // get name of token
    virtual std::string get_name() = 0;

    // get precedence of token
    virtual token_precedence get_precedence() const = 0;

    virtual num_operands_t get_num_operands() = 0;

    virtual util::vector<token_precedence> get_valid_prev_token_prec() = 0;

    virtual 
    util::unique_ptr<token_concept> clone() const = 0;

    // Virtual destructor, to avoid unique_ptr mem leak
    virtual ~token_concept() {}
};

/* Token Template */
// class token_concrete {
// public:
//     static const std::string name;
//     static const token_precedence prec;
// }

template<
    typename tokenT,
    typename calc_process_strategy,
    typename expr_push_strategy,
    typename get_valid_prev_token_strategy,
    typename influence_context_strategy>
class owning_token_model_t: public token_concept {
public:

    explicit owning_token_model_t(
        tokenT in_token,
        calc_process_strategy in_calc_strat,
        expr_push_strategy in_expr_push_strat,
        get_valid_prev_token_strategy in_get_valid_strat,
        influence_context_strategy in_influence_ctx_strat
    ):
        m_token( in_token ),
        m_calc_strat( in_calc_strat ),
        m_expr_push_strat( in_expr_push_strat ),
        m_get_valid_prev_token_strat( in_get_valid_strat ),
        m_influence_ctx_strat( in_influence_ctx_strat )
    {}

    void calc_process(util::stack<double> &st) { 
        m_calc_strat(m_token, st);
    }

    void expr_push(
        util::vector<token> &expr,
        util::stack<token> &st
    ) {
        // expr_push_strategy requires info to build m_token object
        m_expr_push_strat(m_token, expr, st, this);
    }

    util::vector<token_precedence> get_valid_prev_token_prec() {
        return m_get_valid_prev_token_strat(m_token);
    }

    std::string get_name() {
        // is there need for separate strategy??
        return m_token.name;
    }

    token_precedence get_precedence() const {
        // is there need for separate strategy??
        return m_token.prec;
    }

    num_operands_t get_num_operands() {
        // num of operands of m_token (which maybe an operator)
        return m_token.num_operands;
    }

    void influence_ctx(token_conversion_ctx& ctx) {
        m_influence_ctx_strat(m_token, ctx);
    }

    util::unique_ptr<token_concept> clone() const{
        return util::make_unique<owning_token_model_t>( *this );
    }    

private:
    tokenT m_token;
    /*Strategies*/
    calc_process_strategy m_calc_strat;
    expr_push_strategy m_expr_push_strat;
    get_valid_prev_token_strategy m_get_valid_prev_token_strat;
    influence_context_strategy m_influence_ctx_strat;
};

} // namespace detail

// Type-erased token
class token {
public:

    token() {}

    template<
        typename tokenT,
        typename calc_process_strategy,
        typename expr_push_strategy,
        typename get_valid_prev_token_strategy,
        typename influence_context_strategy
    >
    token(
        tokenT tok,
        calc_process_strategy calc_strat,
        expr_push_strategy expr_strat,
        get_valid_prev_token_strategy valid_place_strat,
        influence_context_strategy influence_ctx_strat
    ): pimpl(
        util::make_unique< /*data type*/
            detail::owning_token_model_t<
                tokenT,
                calc_process_strategy,
                expr_push_strategy,
                get_valid_prev_token_strategy,
                influence_context_strategy
            >
        >( /*constructor*/
            tok, calc_strat, expr_strat,
            valid_place_strat, influence_ctx_strat
        )
    ) {}

    // Constructor for cloned token_concept
    token(
        util::unique_ptr<detail::token_concept> pimpl_in
    ):
        pimpl( std::move(pimpl_in) )
    {}

    token(
        const token& other
    ):
        pimpl( other.pimpl->clone() )
    {}

    token&
    operator=(
        const token& other
    ) {
        pimpl = std::move( other.pimpl->clone() );
        return *this;
    }

    void expr_push(
        util::vector<token> &expr,
        util::stack<token> &st
    ) {
        pimpl->expr_push(expr, st);
    }

    void calc_process(util::stack<double> &st) {
        pimpl->calc_process(st);
    }

    // influence context of conversion of sequence of tokens
    void influence_ctx(token_conversion_ctx& ctx) {
        pimpl->influence_ctx(ctx);
    }

    // get vector of valid tokens' precedences
    // which are valid to be placed before *this
    util::vector<token_precedence> get_valid_prev_token_prec() {
        return pimpl->get_valid_prev_token_prec();
    }

    // get_name
    std::string get_name() {
        return pimpl->get_name();
    }

    // get_precedence
    token_precedence get_precedence() const{
        return pimpl->get_precedence();
    }

    bool is_valid_to_place_after(const token& before) {
        util::vector<token_precedence> valid_prev = get_valid_prev_token_prec();
        token_precedence prec = before.get_precedence();

        for(int i = 0; i < valid_prev.size(); ++i)
            if(valid_prev[i] == prec)
                return true;

        return false;
    }

private:
    util::unique_ptr<detail::token_concept> pimpl;
};

// Context of conversion of sequence of tokens
class token_conversion_ctx {
public:
    token_conversion_ctx():
        num_of_commas(0) {}

    bool is_valid() {
        return parenthesis_commas.empty();
    }

    int num_of_commas;
    util::stack<int> parenthesis_commas; /*needed for check of parenthesis and commas*/
};

} // namespace postfix

#endif
