#ifndef POSTFIX_TOKEN_H
#define POSTFIX_TOKEN_H

#include <iostream>

#include <stdexcept>

#include "util/container.h"
#include "util/shared_ptr.h"
#include "util/util.h"

namespace postfix {

typedef enum{
    number,
    left_parenthesis,
    right_paranthesis,
    add_n_sub,
    multiplication,
    unary,
    function
} precedence_t;

// TODO: add precedence and num of ops
class token_base_t {
public:

    virtual void process(util::stack_t<double>& ) = 0;
    virtual void place_to_expr(
        util::shared_ptr<token_base_t> &this_ptr,
        util::vector_t<util::shared_ptr<token_base_t>> &expr,
        util::stack_t<util::shared_ptr<token_base_t>> &st
    ) {
        while(!st.empty())
            if(prec <= st.peek()->prec) {
                expr.push_back(st.peek());
                st.pop();
            } else {
                break;
            }
        st.push(this_ptr);
    }

    virtual ~token_base_t() {}

protected:
    token_base_t(precedence_t precedence): prec(precedence) {}
    static const char * const token_name;
    precedence_t prec;
};


class token_number_t: public token_base_t {
public:
    token_number_t(double num): number(num), token_base_t(precedence_t::number) {}

    void process(util::stack_t<double>& st) {
        st.push(number);
    }
    
    void place_to_expr(
        util::vector_t<util::shared_ptr<token_base_t>> &expr,
        util::stack_t<util::shared_ptr<token_base_t>> &st
    ) {
        // should deal with pointers and etc
        // expr.push_back();
    } 

    virtual ~token_number_t() {}

    static const char * const token_name;
private:
    double number;
};


class token_plus_t: public token_base_t {
public:

    token_plus_t(): token_base_t(precedence_t::add_n_sub) {}

    void process(util::stack_t<double>& st) {
        if(st.size() < 2)
            throw std::domain_error("token_plus: not enough operands");

        double b = st.peek(); st.pop();
        double a = st.peek(); st.pop();
        st.push(a + b);
    }

    virtual ~token_plus_t() {}

    static const char * const token_name;
};

class token_minus_t: public token_base_t {
public:

    token_minus_t(): token_base_t(precedence_t::add_n_sub) {}

    void process(util::stack_t<double>& st) {
        if(st.size() < 2)
            throw std::domain_error("token_minus: not enough operands");

        double b = st.peek(); st.pop();
        double a = st.peek(); st.pop();
        st.push(a - b);
    }

    virtual ~token_minus_t() {}

    static const char * const token_name;
};


class token_unary_minus_t: public token_base_t {
public:

    token_unary_minus_t(): token_base_t(precedence_t::unary) {}

    void process(util::stack_t<double>& st) {
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

    // token_factory_base_t() {
    //     std::cout << "token_factory_base_t::BAZA()" << std::endl;
    // }

    virtual util::shared_ptr<token_base_t> build() = 0;
    virtual const char* get_name() = 0;

    virtual ~token_factory_base_t() {}
};

template<typename token_T>
class token_factory_t: public token_factory_base_t {
public:

    // token_factory_t() {
    //     std::cout << typeid(token_T).name() << "::BAZA()" << std::endl;
    // }

    util::shared_ptr<token_base_t> build() {
        return new token_T();
    }

    const char* get_name() {
        return token_T::token_name;
    }

    virtual ~token_factory_t() {}

};


} // namespace postfix

#endif // POSTFIX_TOKEN_H