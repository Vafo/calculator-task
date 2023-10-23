#include "token.h"

namespace postfix {

void token_base_t::process(util::stack_t<double>& st) {
    // do nothing
    return;
}


const char * const token_base_t::token_name = NULL;
const char * const token_number_t::token_name = NULL;
const char * const token_plus_t::token_name = "+";
const char * const token_minus_t::token_name = "-";

} // namespace postfix