#ifndef UTIL_CONTAINER_H
#define UTIL_CONTAINER_H

#include <algorithm>
#include <cinttypes>
#include <cassert>
#include <memory>

#include "util.h"

namespace postfix::util {

template<typename T>
class vector_t {
public:
    typedef int size_type;
    typedef T* obj_ptr;
    typedef const T* const_obj_ptr;
    typedef T value_type;

    vector_t(): m_raw_ptr(nullptr), m_size(0), m_capacity(0) {}

    vector_t(size_type size): 
        m_raw_ptr(
            allocator.allocate(size)
        ),
        m_size(size),
        m_capacity(size) {
        for(size_type i = 0; i < m_size; ++i)
            allocator.construct(&m_raw_ptr[i]);
    }

    vector_t(size_type size, const T &el): 
        m_raw_ptr(
            allocator.allocate(size)
        ),
        m_size(size),
        m_capacity(size) {
        for(size_type i = 0; i < m_size; ++i)
            allocator.construct(&m_raw_ptr[i], el);
    }

    vector_t(const vector_t &other):
        m_raw_ptr(
            allocator.allocate(other.m_size)
        ),
        m_size(other.m_size),
        m_capacity(other.m_size) {
            copy(other.m_raw_ptr,
                 other.m_size, 
                 m_raw_ptr);
    }

    vector_t(std::initializer_list<T> list):
        m_raw_ptr(
            allocator.allocate(list.size())
        ),
        m_size(list.size()),
        m_capacity(list.size()) {
            copy(
                list.begin(),
                list.size(),
                m_raw_ptr
            );
    }

    vector_t& operator=(vector_t other) {
        swap(*this, other);
        return *this;
    }

    ~vector_t() {
        destroy();
    }

    size_type size() {
        return m_size;
    }

    bool empty() {
        return m_size == 0;
    }

    value_type&
    operator[](size_type n) {
        assert(0 <= n && n < m_size);
        return m_raw_ptr[n];
    }

    bool
    operator==(const vector_t<T>& other) {
        if(m_size != other.m_size)
            return false;

        for(size_t i = 0; i < m_size; ++i)
            if(m_raw_ptr[i] != other.m_raw_ptr[i])
                return false;
        
        return true;
    }

    void push_back(const value_type& el) {
        grow(1);
        allocator.construct(&m_raw_ptr[m_size], el);
        ++m_size;
    }

    void pop_back() {
        if(m_size > 0) {
            allocator.destroy(&m_raw_ptr[m_size-1]);
            --m_size;
        }
    }

    void erase(obj_ptr el) {
        assert(begin() <= el && el < end());
        allocator.destroy(el);
        obj_ptr iter = el;
        while(iter < (end() - 1)) {
            *iter = std::move(*(iter+1));
            iter++;
        }
        m_size--;
    }

    obj_ptr begin() {
        return m_raw_ptr;
    }

    const obj_ptr begin() const {
        return m_raw_ptr;
    }
    
    obj_ptr end() {
        return begin() + m_size;
    }

    const obj_ptr end() const {
        return begin() + m_size;
    }

private:
    std::allocator<T> allocator;
    obj_ptr m_raw_ptr;
    size_type m_size;
    size_type m_capacity;

    void copy(const_obj_ptr src, size_type size, obj_ptr res) {
        for (size_type i = 0; i < size; ++i)
            allocator.construct(&res[i], src[i]);
    }

    void assign(const_obj_ptr src, size_type size, obj_ptr res) {
        for (size_type i = 0; i < size; ++i)
            res[i] = src[i];
    }

    void grow(size_type n) {
        if(m_size + n > m_capacity) {
            size_type new_size = std::max(m_size + n, m_capacity * 2);
            obj_ptr tmp = allocator.allocate(new_size);
            if(m_raw_ptr != NULL) {
                // FIXME: Is there any way to avoid copy, but just move???
                // ANS: Ignore it for now
                std::uninitialized_copy(m_raw_ptr, m_raw_ptr + m_size, tmp);
                destroy();
            }
            m_raw_ptr = tmp;
            m_capacity = new_size;
        }
    }

    void destroy() {
        // checked delete (placed only in 1 place)
        check_if_deletable(m_raw_ptr);
        // Destroy from last to first
        for(size_type i = m_size - 1; i >= 0; --i)
            allocator.destroy(&m_raw_ptr[i]);
        
        allocator.deallocate(m_raw_ptr, m_capacity);
    }

public:
    friend void swap(vector_t &a, vector_t &b) {
        using std::swap;
        swap(a.allocator, b.allocator);
        swap(a.m_capacity, b.m_capacity);
        swap(a.m_size, b.m_size);
        swap(a.m_raw_ptr, b.m_raw_ptr);
    }

};

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