#ifndef UTIL_CONTAINER_H
#define UTIL_CONTAINER_H

#include <algorithm>
#include <cinttypes>
#include <cassert>

namespace postfix::util {

template<typename T>
class vector_t {
public:
    typedef int size_type;
    typedef uint8_t raw;
    typedef raw* raw_ptr;
    typedef const raw* const_raw_ptr;
    typedef T* obj_ptr;
    typedef const T* const_obj_ptr;
    typedef T value_type;

    vector_t(): m_raw_ptr(nullptr), m_size(0), m_capacity(0) {}

    vector_t(size_type size): 
        m_raw_ptr(
            new raw[size * sizeof(T)]
        ),
        m_size(size),
        m_capacity(size) {
        obj_ptr obj = reinterpret_cast<obj_ptr>(m_raw_ptr);
        for(size_type i = 0; i < m_size; ++i)
            new (&obj[i]) T();
    }

    vector_t(size_type size, const T &el): 
        m_raw_ptr(
            new raw[size * sizeof(T)]
        ),
        m_size(size),
        m_capacity(size) {
        obj_ptr obj = reinterpret_cast<obj_ptr>(m_raw_ptr);
        for(size_type i = 0; i < m_size; ++i)
            new (&obj[i]) T(el);
    }

    vector_t(const vector_t &other):
        m_raw_ptr(
            new raw[other.m_size * sizeof(T)]
        ),
        m_size(other.m_size),
        m_capacity(other.m_capacity) {
            copy(other.m_raw_ptr,
                 other.m_size, 
                 m_raw_ptr);
    }

    vector_t(std::initializer_list<T> list):
        m_raw_ptr(
            new raw[list.size() * sizeof(T)]
        ),
        m_size(list.size()),
        m_capacity(list.size()) {
            copy(
                reinterpret_cast<const_raw_ptr>(list.begin()),
                list.size(),
                m_raw_ptr
            );
    }

    vector_t& operator=(const vector_t &other) {
        if(&other != this) {
            if(m_capacity < other.m_size) {
                destroy();
                m_raw_ptr = new raw[other.m_size * sizeof(T)];
                copy(other.m_raw_ptr,
                     other.m_size,
                     m_raw_ptr);

                m_size = other.m_size;
                m_capacity = m_size;
            } else if(m_size >= other.m_size) {
                assign(other.m_raw_ptr,
                       other.m_size,
                       m_raw_ptr);
                
                obj_ptr obj = reinterpret_cast<obj_ptr>(m_raw_ptr);
                for(size_t i = other.m_size; i < m_size; ++i)
                    obj[i].~T();

                m_size = other.m_size;
            } else { // m_size < other.m_size < m_capacity
                assign(other.m_raw_ptr,
                       m_size,
                       m_raw_ptr);
                copy(other.m_raw_ptr + m_size * sizeof(T),
                     other.m_size - m_size, 
                     m_raw_ptr + m_size * sizeof(T));

                m_size = other.m_size;
            }
            
        }

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
        obj_ptr obj = reinterpret_cast<obj_ptr>(m_raw_ptr);
        return obj[n];
    }

    bool
    operator==(const vector_t<T>& other) {
        if(m_size != other.m_size)
            return false;
        
        obj_ptr obj_1 = reinterpret_cast<obj_ptr>(m_raw_ptr);
        obj_ptr obj_2 = reinterpret_cast<obj_ptr>(other.m_raw_ptr);
        for(size_t i = 0; i < m_size; ++i)
            if(obj_1[i] != obj_2[i])
                return false;
        
        return true;
    }

    void push_back(const value_type& el) {
        grow(1);
        obj_ptr obj = reinterpret_cast<obj_ptr>(m_raw_ptr);
        new (&obj[m_size])T(el);
        ++m_size;
    }

    void pop_back() {
        if(m_size > 0) {
            obj_ptr obj = reinterpret_cast<obj_ptr>(m_raw_ptr);
            obj[m_size].~T();
            --m_size;
        }
    }

    obj_ptr begin() {
        return reinterpret_cast<obj_ptr>(m_raw_ptr);
    }

    const obj_ptr begin() const {
        return reinterpret_cast<obj_ptr>(m_raw_ptr);
    }
    
    obj_ptr end() {
        return begin() + m_size;
    }

    const obj_ptr end() const {
        return begin() + m_size;
    }

private:
    raw_ptr m_raw_ptr;
    size_type m_size;
    size_type m_capacity;

    void copy(const_raw_ptr src, size_type size, raw_ptr res) {
        const_obj_ptr obj_src = reinterpret_cast<const_obj_ptr>(src);
        obj_ptr obj_res = reinterpret_cast<obj_ptr>(res);
        for (size_type i = 0; i < size; ++i)
            new (&obj_res[i])T(obj_src[i]);
    }

    void assign(raw_ptr src, size_type size, raw_ptr res) {
        obj_ptr obj_src = reinterpret_cast<obj_ptr>(src);
        obj_ptr obj_res = reinterpret_cast<obj_ptr>(res);
        for (size_type i = 0; i < size; ++i)
            obj_res[i] = obj_src[i];
    }

    void grow(size_type n) {
        if(m_size + n > m_capacity) {
            size_type new_size = std::max(m_size + n, m_capacity * 2);
            raw_ptr tmp = new raw[new_size * sizeof(T)];
            std::copy(m_raw_ptr, m_raw_ptr + m_size * sizeof(T), tmp);
            m_raw_ptr = tmp;
            m_capacity = new_size;
        }
    }

    void destroy() {
        obj_ptr obj = reinterpret_cast<obj_ptr>(m_raw_ptr);
        for(size_type i = 0; i < m_size; ++i)
            obj[i].~T();
        
        delete[] m_raw_ptr;
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

    void push(value_type &obj) {
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

protected:
    Container c;

};

} // namespace postfix::util

#endif