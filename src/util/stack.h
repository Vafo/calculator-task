#ifndef UTIL_STACK_H
#define UTIL_STACK_H

#include "vector.h"

namespace postfix::util {

template<typename T, typename Container = vector<T>>
class stack {
public:
    typedef typename Container::value_type value_type;
    typedef typename Container::size_type size_type;

    stack() { }
    
    stack(std::initializer_list<T> list): c(list) {}

    bool
    operator==(const stack& other) const{
        return c == other.c;
    }

    stack&
    operator=(stack other) {
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
        assert(!c.empty());

        return c[c.size()-1];
    }

    const value_type& peek() const {
        assert(!c.empty());

        return c[c.size()-1];
    }

    size_type size() const {
        return c.size();
    }

    bool empty() const {
        return c.empty();
    }

// swap interface
public:
    friend void swap(stack &a, stack &b) {
        using std::swap;
        
        swap(a.c, b.c);
    }

protected:
    Container c;

};

} // namespace postfix::util

#endif