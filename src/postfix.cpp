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
postfix_converter_impl::to_number(const char *beg, const char *end, double &out_val /*out*/) {
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
postfix_converter_impl::to_operator(
    const char *beg,
    const char *end,
    util::vector<token>& candidate_tokens /*out*/
) {
    assert(beg != NULL && end != NULL);
    if(beg == end)
        return beg;

    while(beg != end && isspace(*beg))
        beg++;

    std::string op_name; // potential operation name
    util::vector<token> found_candidate_tokens;
    
    util::vector<int> candidates(factory_names.size()); // Ideal type would be unordered_set
    std::iota(candidates.begin(), candidates.end(), 0);
    int comp_val;
    int found_idx = -1;

    while(!candidates.empty() && beg != end) {
        // Increment potential operation's name, so that to find exact match
        op_name += *beg;
        beg++;

        // Look through candidates and remove some of them
        // Candidates can be removed easily, as their prefix does not match
        for(
            int *cand_beg = candidates.begin(); 
            cand_beg != candidates.end();
            cand_beg++
        ) {
            if( (comp_val = factory_names[*cand_beg].compare(op_name)) == 0 ) { /*Exact match*/
                found_idx = *cand_beg;
                // push into candidates_of_same_name,
                found_candidate_tokens.push_back( factories[found_idx].build() );
                continue;
            }
            else if (comp_val < 0) { /*not equal prefix*/
                candidates.erase(cand_beg);
                --cand_beg;
            }
        }

        // First found matching operator is returned
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

    // No candidates were found
    // return with no candidates being appended

    return beg;
}

const char*
postfix_converter_impl::get_token_candidates(
    const char* beg,
    const char* end,
    util::vector<token>& candidate_tokens /*out*/
) {
    const char *iter;
    double num;
    // is it number?
    iter = to_number(beg, end, num);
    if(iter != beg) {
        candidate_tokens.push_back(builder::number(num));
        return iter;
    }

    // is it one of operator? (Generally - token)
    iter = to_operator(beg, end, candidate_tokens);
    if(iter != beg) {
        return iter;
    }

    // It is not one of the operators
    std::string err_msg = "postfix_converter_impl: could not convert to token" + std::string(beg);
    throw std::runtime_error(err_msg);
    return NULL;
}

// Find tok, such that it matches with previous token
inline token prune_tokens(
    token& prev_token,
    util::vector<token>& candidate_tokens
) {
    token found_token;
    // find appropriate token among candidates
    bool is_found_valid_token = false;
    for(int i = 0; i < candidate_tokens.size(); ++i) {
        if(candidate_tokens[i].is_valid_to_place_after(prev_token)) {
            is_found_valid_token = true;
            // set found token
            found_token = candidate_tokens[i];
        }
    }

    if(!is_found_valid_token) { /*no valid token was found*/
        std::string err_msg;
        if(!candidate_tokens.empty()) { /*none of them fits after prev_token*/
            err_msg = 
                "postfix_converter::convert: token " +
                candidate_tokens[0].get_name() + " cant be placed after "
                + prev_token.get_name();
        } else {
            err_msg = 
                "postfix_converter::convert: no candidate token was found";
        }

        throw std::logic_error(err_msg);
    }

    return found_token;
}

} // namespace detail


// -a + foo(b*c) * d
// + *
// a-bc*foo d*+
// if 2 conseq operators, just push (??? not valid statement anymore?)

postfix_expr
postfix_converter::convert(const std::string& input) {
    util::stack< token > st;
    postfix_expr postfix;

    // start and end of postfix expr
    std::string edited_input = std::string("(") + input + std::string(")");

    const char 
        *beg = edited_input.begin().base(),
        *end = edited_input.end().base();
    

    util::vector<token> candidate_tokens;
    token cur_token;
    token_conversion_ctx ctx;
    
    // left_parenthesis can be placed after left_parenthesis
    token prev_token = builder::left_parenthesis();
    while(beg != end) {
        // clear candidate tokens, thus reusing alloc mem
        candidate_tokens.clear();
        beg = impl.get_token_candidates(beg, end, candidate_tokens); /*move beg ptr*/
        
        cur_token = detail::prune_tokens(prev_token, candidate_tokens); /*prune tokens by prev_token*/

        // apply token_specific_logic that affects context
        cur_token.influence_ctx(ctx);
        
        // push into expr
        cur_token.expr_push(postfix.expr, st);
        prev_token = cur_token;
    }

    // check if context is valid
    if(!ctx.is_valid())
        throw std::logic_error("invalid parenthesis"); /*might add reason method to ctx*/

    return postfix;
}

double postfix_expr::evaluate() {
    util::stack<double> val_st;
    
    for(int i = 0; i < expr.size(); ++i)
        expr[i].calc_process(val_st);

    // See if it calculated properly
    if(val_st.size() != 1)
        throw std::logic_error("postfix_expr::evaluate(): could not evaluate expression");

    return val_st.peek();
}

} // namespace postfix