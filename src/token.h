#ifndef POSTFIX_TOKEN_H
#define POSTFIX_TOKEN_H

#include <iostream>

#include <stdexcept>

#include "util/container.h"
#include "util/shared_ptr.h"
#include "util/util.h"

namespace postfix {

// TODO: add precedence and num of ops
class token_base_t {
public:
    virtual void process(util::stack_t<double>& );
    static const char * const token_name;
};


class token_number_t: public token_base_t {
public:
    token_number_t(double num): number(num) {}

    void process(util::stack_t<double>& st) {
        st.push(number);
    }
    
    static const char * const token_name;
private:
    double number;
};


class token_plus_t: public token_base_t {
public:
    void process(util::stack_t<double>& st) {
        if(st.size() < 2)
            throw std::domain_error("token_plus: not enough operands");

        double b = st.peek(); st.pop();
        double a = st.peek(); st.pop();
        st.push(a + b);
    }

    static const char * const token_name;
};

class token_minus_t: public token_base_t {
public:
    void process(util::stack_t<double>& st) {
        if(st.size() < 2)
            throw std::domain_error("token_minus: not enough operands");

        double b = st.peek(); st.pop();
        double a = st.peek(); st.pop();
        st.push(a - b);
    }

    static const char * const token_name;
};



// Token Factory

class token_factory_base_t {
public:

    token_factory_base_t() {
        std::cout << "token_factory_base_t::BAZA()" << std::endl;
    }

    virtual util::shared_ptr<token_base_t> build(const char *begin, const char *end) = 0;
    virtual const char* get_name() = 0;
};

template<typename token_T>
class token_factory_t: public token_factory_base_t {
public:

    token_factory_t() {
        std::cout << typeid(token_T).name() << "::BAZA()" << std::endl;
    }

    util::shared_ptr<token_base_t> build(const char *begin, const char *end) {
        return new token_T();
    }

    const char* get_name() {
        return token_T::token_name;
    }

};


template<>
class token_factory_t<token_number_t>: public token_factory_base_t {
public:

    token_factory_t() {
        std::cout << typeid(token_factory_t<token_number_t>).name() << "::BAZA()" << std::endl;
    }

    util::shared_ptr<token_base_t> build(const char *begin, const char *end) {
        double num = 0;
        util::to_number(begin, end, num);
        return new token_number_t(num);
    }

    const char* get_name() {
        return NULL;
    }
};

typedef token_factory_t<token_number_t> token_number_factory_t;

} // namespace postfix

#endif // POSTFIX_TOKEN_H