#ifndef POSTFIX_H
#define POSTFIX_H

#include <algorithm>
#include <string>

#include <iterator>

#include "token.h"
#include "util/vector.h"
#include "util/stack.h"
#include "util/shared_ptr.h"

namespace postfix
{

namespace detail {

class postfix_converter_impl_t {
public:

    postfix_converter_impl_t(
        std::initializer_list<
            util::shared_ptr<token_factory_base_t>
        > list
    ): factories(list) {
        std::sort(factories.begin(), factories.end(), compare_fact);
        std::transform(
            factories.begin(), factories.end(), 
            std::back_inserter(factory_names), get_factory_name
        );
    }

    const char *
    to_token(
        const char *beg,
        const char *end,
        util::shared_ptr<token_base_t> &out_token /*out*/
    );

private:
    util::vector_t< std::string > factory_names;
    util::vector_t< util::shared_ptr<token_factory_base_t> > factories;
    
    // Minus sign is not supported. It is retrieved as separate operator
    const char *
    to_number(const char *beg, const char *end, double &out_val /*out*/);

    const char *
    to_operator(
        const char *beg,
        const char *end,
        util::shared_ptr<token_base_t> &out_oper /*out*/
    );

    static bool compare_fact(
        util::shared_ptr<token_factory_base_t>& a,
        util::shared_ptr<token_factory_base_t>& b
    ) {
        return std::string(a->get_name()) < std::string(b->get_name());
    }

    static std::string get_factory_name( util::shared_ptr<token_factory_base_t> &fact) {
        return std::string(fact->get_name());
    }

#ifdef POSTFIX_TEST
    template<class... Ts> friend class postfix_converter_impl_test;
#endif

};

// Some idea on reducing code bloat
// template<class... Ts>
// util::shared_ptr<token_factory_base_t>
// make_shared_factory_list[] = { 
//         util::shared_ptr<token_factory_base_t> (
//             token_factory_t<Ts>()
//         )...
// };

} // namespace detail

template<class... Ts>
class postfix_converter_t;

class postfix_expr_t {
public:
    double evaluate();

private:
    postfix_expr_t() {}

    util::vector_t< util::shared_ptr<token_base_t> > expr;

    template<class... Ts> friend class postfix_converter_t; 
};

template<class... Ts>
class postfix_converter_t {
public:
    // FIXME: a lot of repetition
    postfix_converter_t():
    impl({
            util::shared_ptr<token_factory_base_t> (
                token_factory_t<token_left_parenthesis_t>()
            ),
            util::shared_ptr<token_factory_base_t> (
                token_factory_t<token_right_parenthesis_t>()
            ),
            util::shared_ptr<token_factory_base_t> (
                token_factory_t<token_comma_t>()
            ),
            util::shared_ptr<token_factory_base_t>(
                token_factory_t<Ts>()
            )...
    }) { }

    postfix_expr_t
    convert(const std::string& input);

private:
    detail::postfix_converter_impl_t impl;

};

// -a + foo(b*c) * d
// + *
// a-bc*foo d*+
// if 2 conseq operators, just push (??? not valid statement anymore?)

template<class... Ts>
postfix_expr_t
postfix_converter_t<Ts...>::convert(const std::string& input) {
    util::stack_t< util::shared_ptr<token_base_t> > st;
    postfix_expr_t postfix;

    // start and end of postfix expr
    std::string edited_input = std::string("(") + input + std::string(")");

    const char 
        *beg = edited_input.begin().base(),
        *end = edited_input.end().base();
    

    util::shared_ptr<token_base_t> token;
    while(beg != end) {
        beg = impl.to_token(beg, end, token);
        // put token into stack
        // apply logic of token of emplacement
        token->expr_push_logic(postfix.expr, st);
    }


    // try to evaluate postfix expr
    // if is executable, return it
    postfix.evaluate();
    // otherwise, throw exception
    return postfix;
}



} // namespace postfix


#endif