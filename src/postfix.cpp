#include "postfix.h"

#include <cstring>
#include <numeric>

#include "token.h"
#include "token_concrete.h"
#include "token_factory.h"
#include "token_builder.h"

#include "util/vector.h"
#include "util/stack.h"
#include "util/shared_ptr.h"

namespace postfix {

namespace detail {

const char *
postfix_converter_impl_t::to_number(const char *beg, const char *end, double &out_val /*out*/) {
    const char *cur_ptr = beg;

    while(cur_ptr != end && isspace(*cur_ptr))
        cur_ptr++;

    const char *start_after_spaces = cur_ptr;
    double res_val = 0;
    while(cur_ptr != end && isdigit(*cur_ptr)) {
        res_val *= 10;
        res_val += (*cur_ptr - '0');
        ++cur_ptr;
    }

    // return to same pos, signaling that no number was found
    if(cur_ptr == start_after_spaces)
        return beg;

    out_val = res_val;
    if(cur_ptr == end || cur_ptr == beg)
        return cur_ptr;
    
    if(*cur_ptr != '.')
        return cur_ptr;
    cur_ptr++; // move 1 position, after dot

    res_val = 0;
    double dividor = 1;
    while(cur_ptr != end && isdigit(*cur_ptr)) {
        dividor /= 10;
        res_val += (*cur_ptr - '0') * dividor;
        ++cur_ptr;
    }

    out_val += res_val;

    return cur_ptr;
}

const char *
postfix_converter_impl_t::to_operator(
    const char *beg,
    const char *end,
    util::vector_t<token_t>& candidate_tokens /*out*/
) {
    assert(beg != NULL && end != NULL);
    if(beg == end)
        return beg;

    while(beg != end && isspace(*beg))
        beg++;

    std::string op_name; // potential operation name
    util::vector_t<token_t> found_candidate_tokens;
    
    util::vector_t<int> candidates(factory_names.size()); // Ideal type would be unordered_set
    std::iota(candidates.begin(), candidates.end(), 0);
    int comp_val;
    int found_idx = -1;

    while(!candidates.empty() && beg != end) {
        op_name += *beg;
        beg++;
        for(
            int *cand_beg = candidates.begin(); 
            cand_beg != candidates.end();
            cand_beg++
        ) {
            if( (comp_val = factory_names[*cand_beg].compare(op_name)) == 0 ) {
                found_idx = *cand_beg;
                // push into candidates_of_same_name,
                found_candidate_tokens.push_back( factories[found_idx].build() );
                continue;
            }
            else if (comp_val < 0) {
                candidates.erase(cand_beg);
                --cand_beg;
            }
        }

        if(!found_candidate_tokens.empty()) {
            // Append candidates to *out* candidates
            // TODO: 
            // Should consider rewriting this code
            // So it would start with preliminary candidates.clear()
            // Thus, saving memory
            std::copy(
                found_candidate_tokens.begin(),
                found_candidate_tokens.end(),
                std::back_inserter(candidate_tokens)
            );

            return beg;
        }
            
    }

    // throw error, as no candidates were found
    std::string err_msg = "postfix_converter_t::to_operator(): \
        no operation candidate was found for " + op_name;
    throw std::runtime_error(err_msg);
    return beg;
}

const char*
postfix_converter_impl_t::to_token(
    const char* beg,
    const char* end,
    util::vector_t<token_t>& candidate_tokens /*out*/
) {
    const char *iter;
    double num;

    iter = to_number(beg, end, num);
    if(iter != beg) {
        candidate_tokens.push_back(builder::number(num));
        return iter;
    }

    iter = to_operator(beg, end, candidate_tokens);
    if(iter != beg) {
        return iter;
    }
    
    std::string err_msg = "postfix_converter_impl_t: could not convert to token" + std::string(beg);
    throw std::runtime_error(err_msg);
    return NULL;
}

} // namespace detail


// -a + foo(b*c) * d
// + *
// a-bc*foo d*+
// if 2 conseq operators, just push (??? not valid statement anymore?)

postfix_expr_t
postfix_converter_t::convert(const std::string& input) {
    util::stack_t< token_t > st;
    postfix_expr_t postfix;

    // start and end of postfix expr
    std::string edited_input = std::string("(") + input + std::string(")");

    const char 
        *beg = edited_input.begin().base(),
        *end = edited_input.end().base();
    

    util::vector_t<token_t> candidate_tokens;
    bool is_found_valid_token;
    // left_parenthesis can be placed after left_parenthesis
    token_t prev_token = builder::left_parenthesis();
    while(beg != end) {
        // clear candidate tokens
        candidate_tokens.clear();
        beg = impl.to_token(beg, end, candidate_tokens);
        

        // std::cout << "Candidates: " << std::endl;
        // for(int i = 0; i < candidate_tokens.size(); ++i)
        //     std::cout << candidate_tokens[i].get_name() << std::endl;

        // find appropriate token
        is_found_valid_token = false;
        for(int i = 0; i < candidate_tokens.size(); ++i) {
            if(candidate_tokens[i].is_valid_to_place_after(prev_token)) {
                is_found_valid_token = true;
                // put token into stack
                candidate_tokens[i].expr_push(postfix.expr, st);
                // set prev_token for next iteration
                prev_token = candidate_tokens[i];
            }
        }

        if(!is_found_valid_token) { /*no valid token was found*/
            std::string err_msg;
            if(!candidate_tokens.empty()) {
                err_msg = 
                    "postfix_converter_t::convert: token " +
                    candidate_tokens[0].get_name() + " cant be placed after "
                    + prev_token.get_name();
            } else {
                err_msg = 
                    "postfix_converter_t::convert: no candidate token was found";
            }

            throw std::logic_error(err_msg);
        }
    }

    return postfix;
}

double postfix_expr_t::evaluate() {
    util::stack_t<double> val_st;
    
    for(int i = 0; i < expr.size(); ++i) {
        
        // {
        //     util::stack_t<double> st_copy = val_st;
        //     while(!st_copy.empty()) {
        //         std::cout << st_copy.peek() << " "; st_copy.pop();
        //     }
        // }
        
        expr[i].calc_process(val_st);
        
        // std::cout << std::endl << "OP " << expr[i].get_name() << std::endl;
        // {
        //     util::stack_t<double> st_copy = val_st;
        //     while(!st_copy.empty()) {
        //         std::cout << st_copy.peek() << " "; st_copy.pop();
        //     }
        // }
    }

    if(val_st.size() != 1)
        throw std::logic_error("postfix_expr_t::evaluate(): could not evaluate expression");

    return val_st.peek();
}

} // namespace postfix