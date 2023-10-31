#include "token_concrete.h"

namespace postfix {

// Allocate memory for const static members of tokens
// Operands
const std::string token_number::name = "";

// Grammar
const std::string token_left_parenthesis::name = "(";
const std::string token_right_parenthesis::name = ")";
const std::string token_comma::name = ",";

// Operators
const std::string token_plus::name = "+";
const std::string token_minus::name = "-";

} // namespace postfix