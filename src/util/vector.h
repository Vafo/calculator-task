#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H


#include <algorithm>
#include <cinttypes>
#include <cassert>
#include <memory>

#include "util.h"

namespace postfix::util {

template<typename T>
class vector {
public:
    typedef ssize_t size_type;
    typedef T* obj_ptr;
    typedef const T* const_obj_ptr;
    typedef T value_type;

    vector(): m_raw_ptr(nullptr), m_size(0), m_capacity(0) {}

    // Constructor: default construct [size] elements
    vector(size_type size): 
        m_raw_ptr(
            allocator.allocate(size)
        ),
        m_size(size),
        m_capacity(size)
    {
        for(size_type i = 0; i < m_size; ++i)
            allocator.construct(&m_raw_ptr[i]);
    }

    // Constructor: copy construct [size] elements from [el]
    vector(size_type size, const T& el): 
        m_raw_ptr(
            allocator.allocate(size)
        ),
        m_size(size),
        m_capacity(size)
    {
        for(size_type i = 0; i < m_size; ++i)
            allocator.construct(&m_raw_ptr[i], el);
    }

    // Constructor: copy construct from [other]
    vector(const vector& other):
        m_raw_ptr(
            allocator.allocate(other.m_size)
        ),
        m_size(other.m_size),
        m_capacity(other.m_size)
    {
        copy_obj(other.m_raw_ptr,
                other.m_size, 
                m_raw_ptr);
    }

    // Constructor: construct from initializer list
    vector(std::initializer_list<T> list):
        m_raw_ptr(
            allocator.allocate(list.size())
        ),
        m_size(list.size()),
        m_capacity(list.size())
    {
        copy_obj(
            list.begin(),
            list.size(),
            m_raw_ptr
        );
    }

    vector& operator=(vector other) {
        swap(*this, other);
        return *this;
    }

    ~vector() {
        destroy();
    }

    size_type size() const {
        return m_size;
    }

    bool empty() const {
        return size() == 0;
    }

    value_type&
    operator[](size_type n) {
        assert(0 <= n && n < m_size);

        return m_raw_ptr[n];
    }

    const value_type&
    operator[](size_type n) const {
        assert(0 <= n && n < m_size);
        
        return m_raw_ptr[n];
    }

    bool
    operator==(const vector<T>& other) const {
        if(m_size != other.m_size)
            return false;

        for(size_t i = 0; i < m_size; ++i)
            if(m_raw_ptr[i] != other.m_raw_ptr[i])
                return false;
        
        return true;
    }

    // Add element to end of vector
    void push_back(const value_type& el) {
        grow(1);
        allocator.construct(&m_raw_ptr[m_size], el);
        ++m_size;
    }

    void pop_back() {
        assert(!empty());

        allocator.destroy(&m_raw_ptr[m_size-1]);
        --m_size;
    }

    // Erase element from vector range
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

    // Clear vector's space but do not free it
    void clear() {
        for(size_type i = 0; i < m_size; ++i)
            allocator.destroy(&m_raw_ptr[i]);

        m_size = 0;
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

    void copy_obj(const_obj_ptr src, size_type size, obj_ptr res) {
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
    friend void swap(vector &a, vector &b) {
        using std::swap;
        swap(a.allocator, b.allocator);
        swap(a.m_capacity, b.m_capacity);
        swap(a.m_size, b.m_size);
        swap(a.m_raw_ptr, b.m_raw_ptr);
    }

};

} // namespace postfix::util

#endif