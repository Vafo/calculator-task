#ifndef UTIL_H
#define UTIL_H

namespace postfix::util {
    
// Returns end of valid range
const char *
to_number(const char *beg, const char *end, double &out_val /*out*/);

} // namespace postfix::util

#endif