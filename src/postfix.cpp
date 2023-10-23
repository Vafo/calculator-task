#include "postfix.h"

#include "util/container.h"
#include "util/shared_ptr.h"

namespace postfix {

const char *
postfix_converter_t::to_operator(
    const char *beg,
    const char *end,
    util::shared_ptr<token_base_t> &out_oper /*out*/
) {

}


// -a + foo(b*c) * d
// + *
// a-bc*foo d*+
// if 2 conseq operators, just push

postfix_expr_t
postfix_converter_t::convert(const std::string& input) {
    char c;
    util::stack_t< util::shared_ptr<token_base_t> > st;

    bool is_number;
    bool is_op_name;


    for(const char *beg = input.begin().base(),
        *end = input.end().base();
        beg != end; 
    ) {
        while(isspace(*beg))
            beg++;
        
        

    }

}


} // namespace postfix