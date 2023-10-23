#include <iostream>
#include <iterator>

#include <string>

#include "postfix.h"
#include "token.h"
#include "container.h"

#include <memory>

// class postfix_expr_t {
//     double calculate();
//     // postfix expression (implied to be valid) 
//     // [!] upon copy & assignment, make it a shared ptr
// };

// enum token_type_t {
//     invalid,
//     num,
//     op,
//     token_type_max
// };

// struct token_t {
//     token_t() {}

//     token_t(std::string val_in, token_type_t t): val(val_in), type(t) {}

//     std::string val;
//     token_type_t type;
// };

// std::string::iterator get_token(
//     std::string::iterator beg,
//     std::string::iterator end,
//     token_t &token_out
// ) {

// }



int main(int argc, char *argv[]) {


    postfix::postfix_converter_t converter(
        {
            postfix::token_number_factory_t(),
            postfix::token_factory_t<postfix::token_plus_t>(),
            postfix::token_factory_t<postfix::token_minus_t>()
        }
    );

    // postfix::util::vector_t< 
    //     postfix::util::shared_ptr<postfix::token_base_t>
    // > expr; 

    // postfix::token_number_factory_t num_fac;
    // expr.push_back(num_fac.build(12));
    // expr.push_back(num_fac.build(5));
    
    // postfix::token_factory_t<
    //     postfix::token_plus_t
    // > plus_fac;
    // expr.push_back(plus_fac.build());

    // std::cout << expr.size() << std::endl;

    // postfix::util::stack_t<double> st;
    // auto beg = expr.begin();
    // auto end = expr.end();
    // while(beg != end) {
    //     std::cout << "A" << std::endl;
    //     (*beg)->process(st);
    //     ++beg;
    // }

    // std::cout << st.size() << " " << st.peek() << std::endl;

    return 0;
}