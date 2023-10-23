#ifndef POSTFIX_H
#define POSTFIX_H

#include <string>
#include <algorithm>

#include <functional>

#include "token.h"
#include "util/container.h"
#include "util/shared_ptr.h"

namespace postfix
{

class postfix_converter_t;

class postfix_expr_t {
public:
    double evaluate();

private:
    postfix_expr_t() {}

    util::vector_t< util::shared_ptr<token_base_t> > postfix;

    friend class postfix_converter_t; 
};


class postfix_converter_t {
public:

    template<class... Ts>
    postfix_converter_t(Ts... args):
    factories({util::shared_ptr<token_factory_base_t>(&args)...}) {
        std::sort(factories.begin(), factories.end(), compare_fact);
    }

    postfix_expr_t
    convert(const std::string& input);

    util::vector_t< util::shared_ptr<token_factory_base_t> > factories;
private:

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
        return a->get_name() < b->get_name();
    }

};


} // namespace postfix


#endif