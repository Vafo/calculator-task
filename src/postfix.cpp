#include "postfix.h"

#include <cstring>
#include <numeric>

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
    util::shared_ptr<token_base_t> &out_oper /*out*/
) {
    assert(beg != NULL && end != NULL);
    if(beg == end)
        return beg;

    while(beg != end && isspace(*beg))
        beg++;

    std::string op_name; // potential operation name
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
                out_oper = factories[found_idx]->build();
                // TODO:
                //      push into candidates_of_same_name,
                //      then loop for exact match of operands
                return beg;
            }
            else if (comp_val < 0) {
                candidates.erase(cand_beg);
                --cand_beg;
            }
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
    util::shared_ptr<token_base_t> &out_token /*out*/
) {
    const char *iter;
    double num;

    iter = to_number(beg, end, num);
    if(iter != beg) {
        token_number_t token_num(num);
        out_token = util::shared_ptr<token_base_t>(token_num);
        return iter;
    }

    util::shared_ptr<token_base_t> tmp_token;
    iter = to_operator(beg, end, tmp_token);
    if(iter != beg) {
        out_token = tmp_token;
        return iter;
    }
    
    std::string err_msg = "postfix_converter_impl_t: could not convert to token" + std::string(beg);
    throw std::runtime_error(err_msg);
    return NULL;
}

} // namespace detail

double postfix_expr_t::evaluate() {
    util::stack_t<double> val_st;
    for(int i = 0; i < expr.size(); ++i) {
        expr[i]->process(val_st);
    }

    if(val_st.size() != 1)
        throw std::logic_error("postfix_expr_t::evaluate(): could not evaluate expression");

    return val_st.peek();
}

} // namespace postfix