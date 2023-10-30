#ifndef UTIL_UNIQUE_PTR_H
#define UTIL_UNIQUE_PTR_H

#include <memory>
#include <cassert>

#include <iostream>

namespace postfix::util {

template<typename T, typename Deleter = std::default_delete<T>>
class unique_ptr {
public:
    unique_ptr(): ptr(nullptr) { }

    unique_ptr(const unique_ptr& other): ptr(NULL) {
        // Deep copy
        if(other.get() != NULL)
            ptr = new T(*other.get());
    }
    // Is there any way to combine alike copy and conversion copy into single copy?
    // unique_ptr(T *in_ptr): ptr(in_ptr) {}
    template<typename D, typename D_Deleter>
    unique_ptr(const unique_ptr<D, D_Deleter>& other): ptr(NULL) {
        // D_Deleter is ignored
        static_assert(std::is_base_of<T, D>::value);
        // Deep copy
        if(other.get() != NULL)
            ptr = new D(*other.get());
    }

    template<typename D>
    unique_ptr(D *in_ptr) {
        static_assert(std::is_base_of<T, D>::value);
        ptr = in_ptr;
    }

    template<typename D, typename D_Deleter>
    unique_ptr& operator=(const unique_ptr<D, D_Deleter> &other) {
        unique_ptr tmp(other);
        this->swap(tmp);

        return *this;
    }

    ~unique_ptr() {
        Deleter deleter;
        deleter(ptr);
    }

    void swap(unique_ptr& other) {
        using std::swap;

        swap(ptr, other.ptr);
    }

    T& operator*() {
        assert(ptr != NULL);
        return *ptr;
    }

    const T& operator*() const {
        assert(ptr != NULL);
        return *ptr;
    }

    T* operator->() {
        assert(ptr != NULL);
        return ptr;
    }

    const T* operator->() const {
        assert(ptr != NULL);
        return ptr;
    }

    void release() {
        reset(NULL);
    }

    void reset(T *new_ptr) {
        Deleter deleter;
        deleter(ptr);
        ptr = new_ptr;
    }

    T* get() {
        return ptr;
    }

    const T* get() const {
        return ptr;
    }

private:
    T *ptr;

};

template<
    typename T,
    typename Deleter = std::default_delete<T>,
    class... Args>
unique_ptr<T, Deleter> make_unique(Args&... args) {
    return unique_ptr<T, Deleter>( new T(args...) );
}

} // namespace postfix::util

#endif