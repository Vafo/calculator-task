#include "postfix.h"

#include <cstring>
#include <numeric>

#include "util/container.h"
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

    switch(*beg) {
        case '(':
            // construct left-brace token
            return beg + 1;
        break;
        case ')':
            // construct right-brace token
            return beg + 1;
        case ',':
            // construct comma token
            return beg + 1;
    }

    // Interval of factory_names, whose prefix equals prefix of input
    std::string 
        *fact_beg = factory_names.begin(),
        *fact_end = factory_names.end(),
        *fact_iter;

    std::string op_name; // potential operation name
    util::vector_t<int> candidates(factory_names.size()); // Ideal type would be unordered_set
    std::iota(candidates.begin(), candidates.end(), 0);
    int comp_val;
    int found_idx = -1;

    while(!candidates.empty()) {
        op_name += *beg;
        beg++;

        for(
            int *cand_beg = candidates.begin(); 
            cand_beg != candidates.end() && beg != end;
            cand_beg++
        ) {
            
            if( (comp_val = factory_names[*cand_beg].compare(op_name)) == 0 ) {
                found_idx = *cand_beg;
                out_oper = factories[found_idx]->build();
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

} // namespace postfix