#ifndef POSTFIX_H
#define POSTFIX_H

#include <algorithm>
#include <string>

#include <iterator>

#include "token.h"
#include "token_concrete.h"
#include "token_factory.h"
#include "token_builder.h"

#include "util/vector.h"
#include "util/stack.h"
#include "util/shared_ptr.h"

namespace postfix
{

namespace detail {

class postfix_converter_impl {
public:

    postfix_converter_impl(
        std::initializer_list<
            token
        > list
    ) {
        std::transform(
            list.begin(), list.end(),
            std::back_inserter(factories), make_token_factory);
        std::sort(factories.begin(), factories.end(), compare_fact);
        std::transform(
            factories.begin(), factories.end(), 
            std::back_inserter(factory_names), get_factory_name
        );
    }

    const char *
    get_token_candidates(
        const char* beg,
        const char* end,
        util::vector<token>& candidate_tokens /*out*/
    );

private:
    util::vector< std::string > factory_names;
    util::vector< token_factory > factories;
    
    // Minus sign is not supported. It is retrieved as separate operator
    const char *
    to_number(const char *beg, const char *end, double &out_val /*out*/);

    const char *
    to_operator(
        const char *beg,
        const char *end,
        util::vector<token>& candidate_tokens /*out*/
    );

    static bool compare_fact(
        token_factory& a,
        token_factory& b
    ) {
        return a.get_name() < b.get_name();
    }

    static std::string get_factory_name(token_factory& fact) {
        return fact.get_name();
    }

    static token_factory make_token_factory(const token& tok) {
        return token_factory(tok);
    }

#ifdef POSTFIX_TEST
    friend class postfix_converter_impl_test;
#endif

};


} // namespace detail

class postfix_expr {
public:
    postfix_expr() {}

    double evaluate();
private:

    util::vector< token > expr;

    friend class postfix_converter; 
};

class postfix_converter {
public:
    // FIXME: a lot of repetition
    postfix_converter():
    impl({ /* initializer-list */
        /*grammar*/
        builder::left_parenthesis(),
        builder::right_paranthesis(),
        builder::comma(),
        /*operators*/
        builder::plus(),
        builder::plus_unary(),
        builder::minus(),
        builder::minus_unary(),
        builder::multiplication(),
        builder::division(),
        /*functions*/
        builder::exp()
    }) { }

    postfix_expr
    convert(const std::string& input);

private:
    detail::postfix_converter_impl impl;

};

} // namespace postfix


#endif