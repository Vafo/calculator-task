#ifndef UTIL_H
#define UTIL_H

namespace postfix::util {

template<typename T>
void check_if_deletable(T *ptr) {
    typedef char type_must_be_complete[ sizeof(T) ? 1 : -1 ];
    (void) sizeof(type_must_be_complete);
}

} // namespace postfix::util

#endif