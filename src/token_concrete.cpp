#include "token_concrete.h"

namespace postfix {

/* Allocated memory for const static members of tokens */

/* Names */
// Operands
const std::string token_number::name = "(number)";

// Grammar
const std::string token_left_parenthesis::name = "(";
const std::string token_right_parenthesis::name = ")";
const std::string token_comma::name = ",";

// Operators
const std::string token_plus::name = "+";
const std::string token_plus_unary::name = "+";
const std::string token_minus::name = "-";
const std::string token_minus_unary::name = "-";
const std::string token_multiplication::name = "*";
const std::string token_division::name = "/";


/* Valid previous tokens */

// Operands
const util::vector_t<precedence_t> token_number::valid_prev_tokens = {
    precedence_t::add_n_sub,
    precedence_t::multiplication,
    precedence_t::unary,
    
    precedence_t::left_parenthesis,
    precedence_t::comma
};

// Grammar
const util::vector_t<precedence_t> token_left_parenthesis::valid_prev_tokens = {
    precedence_t::add_n_sub,
    precedence_t::multiplication,
    precedence_t::unary,
    precedence_t::function,

    precedence_t::left_parenthesis,
    precedence_t::comma
};

const util::vector_t<precedence_t> token_right_parenthesis::valid_prev_tokens = {
    precedence_t::number,

    precedence_t::left_parenthesis,
    precedence_t::right_paranthesis
};

const util::vector_t<precedence_t> token_comma::valid_prev_tokens = {
    precedence_t::number,

    precedence_t::right_paranthesis
};

// Operators
const util::vector_t<precedence_t> token_plus::valid_prev_tokens = {
    precedence_t::number,

    precedence_t::right_paranthesis
};

const util::vector_t<precedence_t> token_plus_unary::valid_prev_tokens = {
    precedence_t::left_parenthesis,
    precedence_t::comma
};

const util::vector_t<precedence_t> token_minus::valid_prev_tokens = {
    precedence_t::number,

    precedence_t::right_paranthesis
};

const util::vector_t<precedence_t> token_minus_unary::valid_prev_tokens = {
    precedence_t::left_parenthesis,
    precedence_t::comma
};

const util::vector_t<precedence_t> token_multiplication::valid_prev_tokens = {
    precedence_t::number,

    precedence_t::right_paranthesis
};

const util::vector_t<precedence_t> token_division::valid_prev_tokens = {
    precedence_t::number,

    precedence_t::right_paranthesis
};

} // namespace postfix