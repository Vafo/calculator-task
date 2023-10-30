#ifndef POSTFIX_TOKEN_H
#define POSTFIX_TOKEN_H

#include <iostream>

#include <stdexcept>

#include "util/vector.h"
#include "util/stack.h"
#include "util/shared_ptr.h"

namespace postfix {

// Forward declaration
class token_base_t;

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

namespace detail {

void do_push_upto_left_paren(
    util::vector_t<util::shared_ptr<token_base_t>> &expr,
    util::stack_t<util::shared_ptr<token_base_t>> &st
);

void do_push_with_precedence(
    precedence_t prec,
    util::vector_t<util::shared_ptr<token_base_t>> &expr,
    util::stack_t<util::shared_ptr<token_base_t>> &st
);

} // namespace detail


struct token_properties_t {
    const char * token_name;
    int num_operands;
};

class token_base_t {
public:

    // Called on evaluation of postfix
    virtual void process(util::stack_t<double>& st) {
        throw std::logic_error("token_base_t::process(): unsupported token is processed");
    }

    // Called on conversion to postfix 
    virtual void expr_push_logic(
        util::vector_t<util::shared_ptr<token_base_t>> &expr,
        util::stack_t<util::shared_ptr<token_base_t>> &st
    ) = 0;

    virtual ~token_base_t() {}

    const int num_ops;
    const precedence_t prec;
protected:
    token_base_t(precedence_t precedence, int num_operands)
    : prec(precedence), num_ops(num_operands) {}
    
    static const char * const token_name;
};


class token_number_t: public token_base_t {
public:
    token_number_t(double num): 
        number(num),
        token_base_t(precedence_t::number, 0) {}

    void process(util::stack_t<double>& st) {
        st.push(number);
    }
    
    void expr_push_logic(
        util::vector_t<util::shared_ptr<token_base_t>> &expr,
        util::stack_t<util::shared_ptr<token_base_t>> &st
    ) {
        // Push itself to expression
        util::shared_ptr<token_base_t> token_copy(*this);
        expr.push_back(token_copy);
        return;
    } 

    virtual ~token_number_t() {}

    static const char * const token_name;
private:
    double number;
};

class token_left_parenthesis_t: public token_base_t {
public:
    token_left_parenthesis_t():
        token_base_t(precedence_t::left_parenthesis, 0) {}

    virtual void expr_push_logic(
        util::vector_t<util::shared_ptr<token_base_t>> &expr,
        util::stack_t<util::shared_ptr<token_base_t>> &st
    ) {
        // Push itself to stack
        util::shared_ptr<token_base_t> token_copy(*this);
        st.push(token_copy);
        return;
    }
    
    virtual ~token_left_parenthesis_t() {}
    static const char * const token_name;
};

class token_right_parenthesis_t: public token_base_t {
public:
    token_right_parenthesis_t():
        token_base_t(precedence_t::right_paranthesis, 0) {}

    virtual void expr_push_logic(
        util::vector_t<util::shared_ptr<token_base_t>> &expr,
        util::stack_t<util::shared_ptr<token_base_t>> &st
    ) {
        detail::do_push_upto_left_paren(expr, st);

        // remove left parenthesis
        st.pop();
        return;
    }

    virtual ~token_right_parenthesis_t() {}
    static const char * const token_name;
};

class token_comma_t: public token_base_t {
public:
    token_comma_t():
        token_base_t(precedence_t::comma, 0) {}
    
    virtual void expr_push_logic(
        util::vector_t<util::shared_ptr<token_base_t>> &expr,
        util::stack_t<util::shared_ptr<token_base_t>> &st
    ) {
        detail::do_push_upto_left_paren(expr, st);
        // do not remove left parenthesis
    }

    virtual ~token_comma_t() {}
    static const char * const token_name;
};

class token_plus_t: public token_base_t {
public:

    token_plus_t():
        token_base_t(precedence_t::add_n_sub, 2) {}

    void process(util::stack_t<double>& st) {
        if(st.size() < 2)
            throw std::domain_error("token_plus: not enough operands");

        double b = st.peek(); st.pop();
        double a = st.peek(); st.pop();
        st.push(a + b);
    }

    virtual void expr_push_logic(
        util::vector_t<util::shared_ptr<token_base_t>> &expr,
        util::stack_t<util::shared_ptr<token_base_t>> &st
    ) {
        detail::do_push_with_precedence(this->prec, expr, st);

        // Push itself to stack
        util::shared_ptr<token_base_t> token_copy(*this);
        st.push(token_copy);
    }

    virtual ~token_plus_t() {}

    static const char * const token_name;
};

class token_minus_t: public token_base_t {
public:

    token_minus_t():
        token_base_t(precedence_t::add_n_sub, 2) {}

    virtual void process(util::stack_t<double>& st) {
        if(st.size() < 2)
            throw std::domain_error("token_minus: not enough operands");

        double b = st.peek(); st.pop();
        double a = st.peek(); st.pop();
        st.push(a - b);
    }

    virtual void expr_push_logic(
        util::vector_t<util::shared_ptr<token_base_t>> &expr,
        util::stack_t<util::shared_ptr<token_base_t>> &st
    ) {
        detail::do_push_with_precedence(this->prec, expr, st);

        // Push itself to stack
        util::shared_ptr<token_base_t> token_copy(*this);
        st.push(token_copy);
    }

    virtual ~token_minus_t() {}

    static const char * const token_name;
};


class token_unary_minus_t: public token_base_t {
public:

    token_unary_minus_t():
        token_base_t(precedence_t::unary, 1) {}

    virtual void process(util::stack_t<double>& st) {
        if(st.size() < 1)
            throw std::domain_error("token_unary_minus: not enough operands");

        double a = st.peek(); st.pop();
        st.push(-a);
    }

    virtual ~token_unary_minus_t() {}

    static const char * const token_name;
};


// Token Factory

class token_factory_base_t {
public:

    virtual util::shared_ptr<token_base_t> build() = 0;
    virtual const char* get_name() = 0;

    virtual ~token_factory_base_t() {}
};

template<typename token_T>
class token_factory_t: public token_factory_base_t {
public:

    virtual util::shared_ptr<token_base_t> build() {
        return new token_T();
    }

    virtual const char* get_name() {
        return token_T::token_name;
    }

    ~token_factory_t() {}

};


} // namespace postfix

#endif // POSTFIX_TOKEN_H