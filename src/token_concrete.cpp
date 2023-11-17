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

const std::string token_exp::name = "exp";


/* Valid previous tokens */

// Operands
const util::vector<token_precedence> token_number::valid_prev_tokens = {
    token_precedence::add_n_sub,
    token_precedence::multiplication,
    token_precedence::unary,
    
    token_precedence::left_parenthesis,
    token_precedence::comma
};

// Grammar
const util::vector<token_precedence> token_left_parenthesis::valid_prev_tokens = {
    token_precedence::add_n_sub,
    token_precedence::multiplication,
    token_precedence::unary,
    token_precedence::function,

    token_precedence::left_parenthesis,
    token_precedence::comma
};

const util::vector<token_precedence> token_right_parenthesis::valid_prev_tokens = {
    token_precedence::number,

    token_precedence::left_parenthesis,
    token_precedence::right_paranthesis
};

const util::vector<token_precedence> token_comma::valid_prev_tokens = {
    token_precedence::number,

    token_precedence::right_paranthesis
};

// Operators
const util::vector<token_precedence> token_plus::valid_prev_tokens = {
    token_precedence::number,

    token_precedence::right_paranthesis
};

const util::vector<token_precedence> token_plus_unary::valid_prev_tokens = {
    token_precedence::left_parenthesis,
    token_precedence::comma
};

const util::vector<token_precedence> token_minus::valid_prev_tokens = {
    token_precedence::number,

    token_precedence::right_paranthesis
};

const util::vector<token_precedence> token_minus_unary::valid_prev_tokens = {
    token_precedence::left_parenthesis,
    token_precedence::comma
};

const util::vector<token_precedence> token_multiplication::valid_prev_tokens = {
    token_precedence::number,

    token_precedence::right_paranthesis
};

const util::vector<token_precedence> token_division::valid_prev_tokens = {
    token_precedence::number,

    token_precedence::right_paranthesis
};

const util::vector<token_precedence> token_exp::valid_prev_tokens = {
    token_precedence::add_n_sub,
    token_precedence::multiplication,
    token_precedence::unary,
    
    token_precedence::left_parenthesis,
    token_precedence::comma
};

} // namespace postfix