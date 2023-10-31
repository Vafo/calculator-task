#ifndef UTIL_STACK_H
#define UTIL_STACK_H

#include "vector.h"

namespace postfix::util {

template<typename T, typename Container = vector_t<T>>
class stack_t {
public:
    typedef typename Container::value_type value_type;
    typedef typename Container::size_type size_type;

    stack_t() { }
    
    stack_t(std::initializer_list<T> list): c(list) {}

    bool
    operator==(const stack_t& other) {
        return c == other.c;
    }

    stack_t&
    operator=(stack_t other) {
        swap(*this, other);
        return *this;
    }

    void push(const value_type &obj) {
        c.push_back(obj);
    }

    void pop() {
        c.pop_back();
    }

    value_type& peek() {
        assert(c.size() != 0);
        return c[c.size()-1];
    }

    size_type size() {
        return c.size();
    }

    bool empty() {
        return c.empty();
    }

// swap interface
public:
    friend void swap(stack_t &a, stack_t &b) {
        using std::swap;
        
        swap(a.c, b.c);
    }

protected:
    Container c;

};

} // namespace postfix::util

#endif